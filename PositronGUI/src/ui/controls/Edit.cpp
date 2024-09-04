#include "ui/controls/Edit.hpp"

#include "helpers/HelperFunctions.hpp"
#include "ui/Colors.hpp"
#include "factories/WICFactory.hpp"

#include <cwctype>
#include <type_traits>
#include <strsafe.h>
#include <TOM.h>


namespace Msftedit
{
	using CreateTextServicesFunc = HRESULT(_stdcall*)(IUnknown*, ITextHost*, IUnknown**);
	using ShutdownTextServicesFunc = HRESULT(_stdcall*)(IUnknown*);

	HMODULE msftedit = nullptr;
	CreateTextServicesFunc createTextServices = nullptr;
	ShutdownTextServicesFunc shutdownTextServices = nullptr;
	IID* IIDITextServices2;
	IID* IIDITextHost;
	IID* IIDITextHost2;

	void LoadMsftedit()
	{
		if (!msftedit)
		{
			msftedit = LoadLibraryW(L"Msftedit.dll");
		}
		if (msftedit == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}

		if (!createTextServices)
		{
			createTextServices = std::bit_cast<CreateTextServicesFunc>(GetProcAddress(msftedit, "CreateTextServices"));
		}
		if (createTextServices == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}

		if (!shutdownTextServices)
		{
			shutdownTextServices = std::bit_cast<ShutdownTextServicesFunc>(GetProcAddress(msftedit, "ShutdownTextServices"));
		}
		if (shutdownTextServices == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}
		
		if (!IIDITextServices2)
		{
			IIDITextServices2 = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextServices2"));
		}
		if (IIDITextServices2 == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}

		if (!IIDITextHost)
		{
			IIDITextHost = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextHost"));
		}
		if (IIDITextHost == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}

		if (!IIDITextHost2)
		{
			IIDITextHost2 = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextHost2"));
		}
		if (IIDITextHost2 == nullptr)
		{
			PGUI::HR_T(PGUI::HresultFromWin32());
		}
	}
}

namespace PGUI::UI::Controls
{
	auto Edit::PixelsToTwips(long pixels) noexcept -> long
	{
		return pixels * 15;
	}

	Edit::Edit(const EditParams& params) :
		Control{ Core::WindowClass::Create(L"Edit_UIControl", CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW) },
		passwordChar{ params.passwordChar }, propertyBits{ params.propertyBits | TXTBIT_D2DDWRITE | TXTBIT_D2DSUBPIXELLINES }
	{
		textHost.parentWindow = this;

		RegisterMessageHandler(WM_CREATE, &Edit::OnCreate);
		RegisterMessageHandler(WM_DESTROY, &Edit::OnDestroy);
		RegisterMessageHandler(WM_PAINT, &Edit::OnPaint);
		RegisterMessageHandler(WM_SETFOCUS, &Edit::OnSetFocus);
		RegisterMessageHandler(WM_KILLFOCUS, &Edit::OnKillFocus);
		RegisterMessageHandler(WM_SETCURSOR, &Edit::OnSetCursor);

		for (int msg = WM_KEYFIRST; msg <= WM_KEYLAST; msg++)
		{
			RegisterMessageHandler(msg, &Edit::ForwardToTextServices);
		}
		for (int msg = WM_MOUSEFIRST; msg <= WM_MOUSELAST; msg++)
		{
			RegisterMessageHandler(msg, &Edit::ForwardToTextServices);
		}
		RegisterMessageHandler(WM_COPY, &Edit::ForwardToTextServices);
		RegisterMessageHandler(WM_CUT, &Edit::ForwardToTextServices);
		RegisterMessageHandler(WM_CLEAR, &Edit::ForwardToTextServices);
		RegisterMessageHandler(WM_PASTE, &Edit::ForwardToTextServices);
		RegisterMessageHandler(WM_DROPFILES, &Edit::ForwardToTextServices);
		
		charFormat.yHeight = PixelsToTwips(params.fontSize);
		StringCchCopyW(charFormat.szFaceName, params.fontFace.length(), params.fontFace.c_str());

		if (UIColors::IsDarkMode())
		{
			charFormat.crTextColor = Colors::Aliceblue;
			backgroundBrush.SetParameters(0x1b1b1b);
			charFormat.crBackColor = std::get<RGBA>(backgroundBrush.GetParameters());
		}
		else
		{
			charFormat.crTextColor = UIColors::GetForegroundColor();
			backgroundBrush.SetParameters(UIColors::GetBackgroundColor());
			charFormat.crBackColor = std::get<RGBA>(backgroundBrush.GetParameters());
		}

		Msftedit::LoadMsftedit();
	}

	void Edit::SetText(std::wstring_view text) const noexcept
	{
		textServices->TxSetText(text.data());
		Invalidate();
	}

	auto Edit::GetText() const noexcept -> std::wstring
	{
		std::wstring text(GetTextLength(), L'\0');

		HRESULT hr = textServices->TxSendMessage(WM_GETTEXT, text.length() + 1,
			std::bit_cast<LPARAM>(text.data()), nullptr);
		HR_L(hr);

		return text;
	}

	void Edit::SetKeyFilter(const KeyFilterFunction& keyFilter) noexcept
	{
		filteringFunction = keyFilter;
	}
	void Edit::RemoveKeyFilter() noexcept
	{
		filteringFunction = nullptr;
	}

	void Edit::SetTextColor(RGBA color) noexcept
	{
		charFormat.crTextColor = color;
		SetDefaultCharFormat(charFormat);
	}

	void Edit::SetFontSize(long fontSize) noexcept
	{
		charFormat.yHeight = PixelsToTwips(fontSize);
		SetDefaultCharFormat(charFormat);
	}

	void Edit::SetFontFace(std::wstring_view fontFace) noexcept
	{
		StringCchCopyW(charFormat.szFaceName, fontFace.length(), fontFace.data());
		SetDefaultCharFormat(charFormat);
	}

	auto Edit::GetCaretPosition() const noexcept -> PointL
	{
		return textHost.caretPos;
	}

	auto Edit::GetCaretCharIndex() const noexcept -> std::int64_t
	{
		return GetCharIndexFromPosition(GetCaretPosition());
	}

	#pragma region RICH_EDIT_IMPL

	void Edit::SetPasswordChar(wchar_t passChar) noexcept
	{
		passwordChar = passChar;
		Invalidate();
	}

	void Edit::SetBackgroundBrush(const Brush& bkgndBrush) noexcept
	{
		backgroundBrush.SetParameters(bkgndBrush.GetParameters());
	}

	auto Edit::CanPaste() const noexcept -> bool
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CANPASTE, NULL, NULL, &ret); HR_L(hr);
		
		return ret;
	}
	auto Edit::CanRedo() const noexcept -> bool
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CANREDO, NULL, NULL, &ret); HR_L(hr);
		
		return ret;
	}
	auto Edit::CanUndo() const noexcept -> bool
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CANUNDO, NULL, NULL, &ret); HR_L(hr);

		return ret;
	}

	void Edit::Clear() const noexcept
	{
		HRESULT hr = textServices->TxSendMessage(WM_CLEAR, NULL, NULL, nullptr); HR_L(hr);
	}
	void Edit::Copy() const noexcept
	{
		HRESULT hr = textServices->TxSendMessage(WM_COPY, NULL, NULL, nullptr); HR_L(hr);
	}
	void Edit::Cut() const noexcept
	{
		HRESULT hr = textServices->TxSendMessage(WM_CUT, NULL, NULL, nullptr); HR_L(hr);
	}
	void Edit::Paste() const noexcept
	{
		HRESULT hr = textServices->TxSendMessage(WM_PASTE, NULL, NULL, nullptr); HR_L(hr);
	}
	void Edit::PasteSpecial(UINT clipFormat, DWORD aspect, HMETAFILE metaFile) const noexcept
	{
		REPASTESPECIAL pasteSpecial{ };
		pasteSpecial.dwAspect = aspect;
		pasteSpecial.dwParam = std::bit_cast<DWORD_PTR>(metaFile);

		HRESULT hr = textServices->TxSendMessage(EM_PASTESPECIAL, clipFormat,
			std::bit_cast<LPARAM>(&pasteSpecial), nullptr); HR_L(hr);
	}

	auto Edit::GetCharIndexFromPosition(PointL position) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CHARFROMPOS, NULL, 
			std::bit_cast<LPARAM>(&position), &ret); HR_L(hr);

		return ret;
	}
	auto Edit::GetPositionFromCharIndex(std::int64_t index) const noexcept -> PointL
	{
		POINT p{ };

		HRESULT hr =
			textServices->TxSendMessage(EM_POSFROMCHAR, std::bit_cast<WPARAM>(p), index, nullptr);
		HR_L(hr);

		return p;
	}

	auto Edit::DisplayBand(RectL displayRect) const noexcept -> BOOL
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_DISPLAYBAND, NULL,
			std::bit_cast<LPARAM>(&displayRect), &ret); HR_L(hr);

		return static_cast<BOOL>(ret);
	}

	void Edit::EmptyUndoBuffer() const noexcept
	{
		HRESULT hr = 
			textServices->TxSendMessage(EM_EMPTYUNDOBUFFER, NULL, NULL, nullptr); 
		HR_L(hr);
	}

	auto Edit::IsModified() const noexcept -> bool
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_GETMODIFY, NULL,
			NULL, &ret); 
		HR_L(hr);

		return static_cast<bool>(ret);
	}
	void Edit::SetModified(bool modified) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETMODIFY, modified, NULL, nullptr);
		HR_L(hr);
	}

	auto Edit::IsReadOnly() const noexcept -> bool
	{
		return propertyBits & TXTBIT_READONLY;
	}

	void Edit::SetReadOnly(bool readonly) noexcept
	{
		if (readonly)
		{
			propertyBits |= TXTBIT_READONLY;
			return;
		}
		else
		{
			propertyBits &= ~TXTBIT_READONLY;
		}
		textServices->OnTxPropertyBitsChange(propertyBits, propertyBits);
	}

	auto Edit::IsPassword() const noexcept -> bool
	{
		return propertyBits & TXTBIT_USEPASSWORD;
	}

	void Edit::SetPassword(bool password) noexcept
	{
		if (password)
		{
			propertyBits |= TXTBIT_USEPASSWORD;
			return;
		}
		else
		{
			propertyBits &= ~TXTBIT_USEPASSWORD;
		}
		textServices->OnTxPropertyBitsChange(propertyBits, propertyBits);
	}

	auto Edit::Find(EditFindFlag flags, std::wstring_view text, CharRange searchRange) const noexcept -> CharRange
	{
		FINDTEXTEXW ft{ };
		ft.chrg = searchRange;
		ft.lpstrText = text.data();

		HRESULT hr =
			textServices->TxSendMessage(EM_FINDTEXTEXW, 
				static_cast<WPARAM>(flags), 
				std::bit_cast<LPARAM>(&ft), nullptr);
		HR_L(hr);
		return ft.chrgText;
	}

	auto Edit::FindWordBreak(FindWordBreakOperations op, std::int64_t startPosition) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		
		HRESULT hr =
			textServices->TxSendMessage(EM_FINDWORDBREAK, 
				static_cast<WPARAM>(op), startPosition, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::FormatRange(std::optional<FORMATRANGE> formatRange, bool display) const noexcept -> std::int64_t
	{
		const FORMATRANGE* formatRangePtr = nullptr;

		if (formatRange.has_value())
		{
			formatRangePtr = &formatRange.value();
		}

		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_FORMATRANGE, display, std::bit_cast<LPARAM>(formatRangePtr), &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetEventMask() const noexcept -> EditEventMaskFlag
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETEVENTMASK, NULL, NULL, &ret);
		HR_L(hr);

		return static_cast<EditEventMaskFlag>(ret);
	}
	void Edit::SetEventMask(EditEventMaskFlag eventFlag) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETEVENTMASK, NULL, 
				static_cast<LPARAM>(eventFlag), nullptr);
		HR_L(hr);
	}

	auto Edit::GetFirstVisibleLine() const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETFIRSTVISIBLELINE, NULL, NULL, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetRichEditOle() const noexcept -> ComPtr<IRichEditOle>
	{
		ComPtr<IRichEditOle> ole;

		HRESULT hr =
			textServices->TxSendMessage(EM_GETOLEINTERFACE, NULL, 
				std::bit_cast<LPARAM>(ole.GetAddressOf()), nullptr);
		HR_L(hr);
		
		return ole;
	}

	auto Edit::GetTextLimit() const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETLIMITTEXT, NULL,
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetOptions() const noexcept -> EditOptionsFlag
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETOPTIONS, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<EditOptionsFlag>(ret);
	}
	void Edit::SetOptions(EditOptionsFlag options) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_GETOPTIONS, NULL,
				static_cast<LPARAM>(options), nullptr);
		HR_L(hr);
	}

	void Edit::SetParaFormat(const PARAFORMAT2& pf) noexcept
	{
		paraFormat = pf;

		HRESULT hr =
			textServices->TxSendMessage(EM_SETPARAFORMAT, NULL,
				std::bit_cast<LPARAM>(&paraFormat), nullptr);
		HR_L(hr);
	}

	auto Edit::GetFormattingRect() const noexcept -> RectL
	{
		RECT rc{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETRECT, NULL,
				std::bit_cast<LPARAM>(&rc), nullptr);
		HR_L(hr);

		return rc;
	}
	void Edit::SetFormattingRect(RectL rect) const noexcept
	{
		RECT rc = rect;
		HRESULT hr =
			textServices->TxSendMessage(EM_SETRECT, 0,
				std::bit_cast<LPARAM>(&rc), nullptr);
		HR_L(hr);
	}

	auto Edit::GetRedoName() const noexcept -> UNDONAMEID
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETREDONAME, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<UNDONAMEID>(ret);
	}
	auto Edit::GetUndoName() const noexcept -> UNDONAMEID
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETUNDONAME, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<UNDONAMEID>(ret);
	}

	auto Edit::GetSelection() const noexcept -> CharRange
	{
		CharRange charRange{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_EXGETSEL, NULL,
				std::bit_cast<LPARAM>(&charRange), nullptr);
		HR_L(hr);

		return charRange;
	}
	void Edit::SetSelection(CharRange charRange) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_EXSETSEL, NULL,
				std::bit_cast<LPARAM>(&charRange), nullptr);
		HR_L(hr);
	}

	auto Edit::GetSelectionCharFormat() const noexcept -> CHARFORMAT2W
	{
		CHARFORMAT2W cf{ };
		cf.cbSize = sizeof(CHARFORMAT2W);

		HRESULT hr =
			textServices->TxSendMessage(EM_GETCHARFORMAT, SCF_SELECTION,
				std::bit_cast<LPARAM>(&cf), nullptr);
		HR_L(hr);

		return cf;
	}

	void Edit::SetSelectionCharFormat(const CHARFORMAT2W& cf) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION,
				std::bit_cast<LPARAM>(&cf), nullptr);
		HR_L(hr);
	}

	void Edit::SetWordCharFormat(const CHARFORMAT2W& cf) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETCHARFORMAT, SCF_WORD,
				std::bit_cast<LPARAM>(&cf), nullptr);
		HR_L(hr);
	}

	auto Edit::GetSelectionType() const noexcept -> EditSelectionFlag
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_SELECTIONTYPE, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<EditSelectionFlag>(ret);
	}

	auto Edit::GetSelectedText() const noexcept -> std::wstring
	{
		auto selectionSize = GetSelection().Length() + 1;

		std::wstring selectedText(selectionSize, L'\0');

		HRESULT hr =
			textServices->TxSendMessage(EM_GETSELTEXT, NULL,
				std::bit_cast<LPARAM>(selectedText.data()), nullptr);
		HR_L(hr);

		return selectedText;
	}

	auto Edit::GetTextLength(std::optional<GETTEXTLENGTHEX> lengthEx) const noexcept -> std::int64_t
	{
		if (!lengthEx.has_value())
		{
			LRESULT ret{ };
			HRESULT hr =
				textServices->TxSendMessage(WM_GETTEXTLENGTH, NULL,
					NULL, &ret);
			HR_L(hr);

			return ret;
		}
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETTEXTLENGTHEX, std::bit_cast<WPARAM>(&(lengthEx.value())),
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetTextMode() const noexcept -> TEXTMODE
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETTEXTMODE, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<TEXTMODE>(ret);
	}
	void Edit::SetTextMode(TEXTMODE textMode) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETTEXTMODE, textMode,
				NULL, nullptr);
		HR_L(hr);
	}

	auto Edit::GetTextRange(CharRange charRange) const noexcept -> std::wstring
	{
		std::wstring text(charRange.Length(), L'\0');

		TEXTRANGEW tr{ };
		tr.chrg = charRange;
		tr.lpstrText = text.data();

		HRESULT hr =
			textServices->TxSendMessage(EM_GETTEXTRANGE, NULL,
				std::bit_cast<LPARAM>(&tr), nullptr);
		HR_L(hr);

		return text;
	}

	void Edit::SetTargetDevice(HDC hdc, std::int64_t lineWidth) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETTARGETDEVICE, 
				std::bit_cast<WPARAM>(hdc),
				lineWidth, nullptr);
		HR_L(hr);
	}

	void Edit::SetUndoLimit(std::int64_t undoLimit) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETUNDOLIMIT, undoLimit,
				NULL, nullptr);
		HR_L(hr);
	}

	void Edit::ShowSelection() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_HIDESELECTION, false,
				NULL, nullptr);
		HR_L(hr);
	}
	void Edit::HideSelection() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_HIDESELECTION, true,
				NULL, nullptr);
		HR_L(hr);
	}
	void Edit::ReplaceSelection(std::wstring_view newText, bool canUndo) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_REPLACESEL, canUndo,
				std::bit_cast<LPARAM>(newText.data()), nullptr);
		HR_L(hr);
	}

	void Edit::SetTextLimit(std::int64_t limit) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_EXLIMITTEXT, NULL,
				limit, nullptr);
		HR_L(hr);
	}

	auto Edit::GetLine(int line) const noexcept -> std::wstring
	{
		auto length = GetLineLength(GetLineIndex(line));

		std::wstring lineText(length, L'\0');

		HRESULT hr =
			textServices->TxSendMessage(EM_GETLINE, line,
				std::bit_cast<LPARAM>(lineText.data()), nullptr);
		HR_L(hr);

		return lineText;
	}

	void Edit::StopGroupTyping() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_STOPGROUPTYPING, NULL,
				NULL, nullptr);
		HR_L(hr);
	}

	auto Edit::GetLineFromCharIndex(std::int64_t index) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_EXLINEFROMCHAR, NULL,
				index, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetLineIndex(std::int64_t line) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_LINEINDEX, line,
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	auto Edit::GetLineLength(std::int64_t index) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_LINELENGTH, index,
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	void Edit::ScrollLines(std::int64_t lines) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_LINESCROLL, NULL,
				lines, nullptr);
		HR_L(hr);
	}

	void Edit::RequestSize() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_REQUESTRESIZE, NULL,
				NULL, nullptr);
		HR_L(hr);
	}

	void Edit::SetOleCallack(ComPtr<IRichEditOleCallback> callback) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETOLECALLBACK, NULL,
				std::bit_cast<LPARAM>(callback.Get()), nullptr);
		HR_L(hr);
	}

	void Edit::EnableAutoURLDetect() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_AUTOURLDETECT, NULL,
				AURL_ENABLEEAURLS, nullptr);
		HR_L(hr);
	}
	void Edit::DisableAutoURLDetect() const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_AUTOURLDETECT, NULL,
				0, nullptr);
		HR_L(hr);
	}

	auto Edit::StreamIn(int streamFormat, EDITSTREAM editStream) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_STREAMIN, streamFormat,
				std::bit_cast<LPARAM>(&editStream), &ret);
		HR_L(hr);

		return ret;
	}
	auto Edit::StreamOut(int streamFormat, EDITSTREAM editStream) const noexcept -> std::int64_t
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_STREAMOUT, streamFormat,
				std::bit_cast<LPARAM>(&editStream), &ret);
		HR_L(hr);

		return ret;
	}

	void Edit::SetDefaultCharFormat(const CHARFORMAT2W& cf) noexcept
	{
		charFormat = cf;
		HRESULT hr =
			textServices->TxSendMessage(EM_SETCHARFORMAT, SPF_SETDEFAULT,
				std::bit_cast<LPARAM>(&charFormat), nullptr);
		HR_L(hr);
	}

	#pragma endregion

	void Edit::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
		}
	}

	void Edit::DiscardDeviceResources()
	{
		backgroundBrush.ReleaseBrush();
	}

	void Edit::CaretBlinkHandler(Core::TimerId)
	{
		showCaret = !showCaret;
		Invalidate();
	}

	auto Edit::OnDPIChange(float dpiScale, RectI suggestedRect) -> Core::HandlerResult
	{
		textServices->TxSendMessage(EM_SETZOOM, GetDPI(), PGUI::Core::DEFAULT_SCREEN_DPI, nullptr);

		return Window::OnDPIChange(dpiScale, suggestedRect);
	}

	auto Edit::ForwardToTextServices(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult
	{
		if (!textServices)
		{
			return DefWindowProcW(Hwnd(), msg, wParam, lParam);
		}

		if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST
			&& filteringFunction)
		{
			if (auto process = std::invoke(filteringFunction, this, msg, wParam, lParam);
				!process)
			{
				return DefWindowProcW(Hwnd(), msg, wParam, lParam);
			}
		}

		LRESULT result{ };

		if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
		{
			auto p = std::bit_cast<LPPOINTS>(&lParam);
			p->x = ScaleByDPI(p->x);
			p->y = ScaleByDPI(p->y);
		}
		
		if (HRESULT hr = textServices->TxSendMessage(msg, wParam, lParam, &result); 
			hr == E_OUTOFMEMORY)
		{
			HR_T(hr);
		}

		return result;
	}

	auto Edit::OnCreate(UINT, WPARAM, LPARAM lParam) -> Core::HandlerResult
	{
		const auto clientRect = GetClientRect();
		const auto clientSize = clientRect.Size();

		auto params = ScrollBar::ScrollBarParams{ 5, 0, 5 };
		verticalScrollBar = AddChildWindow<ScrollBar>(
			Core::WindowCreateParams{ L"Edit_VerticalScrollBar", 
			{ clientSize.cx - 20, 0 }, { 20, clientSize.cy },
			NULL },
			params
		);

		params.direction = ScrollBarDirection::Horizontal;
		horizontalScrollBar = AddChildWindow<ScrollBar>(
			Core::WindowCreateParams{ L"Edit_HorizontalScrollBar",
			{ 0, clientSize.cy - 20 }, { clientSize.cx, 20 },
			NULL },
			params
		);

		charFormat.cbSize = sizeof(CHARFORMAT2W);
		charFormat.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;

		paraFormat.cbSize = sizeof(PARAFORMAT2);

		ComPtr<IUnknown> unk;
		HRESULT hr = Msftedit::createTextServices(nullptr, &textHost, &unk); HR_T(hr);
		hr = unk->QueryInterface(*Msftedit::IIDITextServices2, 
			std::bit_cast<void**>(textServices.GetAddressOf())); HR_T(hr);

		textServices->TxSendMessage(EM_SETTYPOGRAPHYOPTIONS,
			0x1000 | 0x2000,
			0x1000 | 0x2000, nullptr);
		textServices->OnTxInPlaceActivate(nullptr);

		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		textServices->TxSetText(createStruct->lpszName);

		textServices->TxSendMessage(EM_SETZOOM, GetDPI(), PGUI::Core::DEFAULT_SCREEN_DPI, nullptr);

		return 0;
	}

	auto Edit::OnDestroy(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		HRESULT hr = Msftedit::shutdownTextServices(textServices.Get()); HR_L(hr);
		textServices.Detach();
		return 0;
	}

	auto Edit::OnPaint(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		BeginDraw();

		auto g = GetGraphics();

		g.Clear(backgroundBrush);

		RECT bounds = GetClientRect();
		bounds.right -= verticalScrollBar->IsVisible() ? ScaleByDPI(20) : 0;
		bounds.bottom -= horizontalScrollBar->IsVisible() ? ScaleByDPI(20) : 0;

		textServices->TxDrawD2D(g, std::bit_cast<LPRECTL>(&bounds), nullptr, TXTVIEW_ACTIVE);

		if (textHost.caretRenderTarget && showCaret)
		{
			auto bmp = textHost.caretRenderTarget.GetBitmap();

			g->DrawImage(bmp, textHost.caretPos, RectF{ {}, bmp->GetSize() },
				D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_MASK_INVERT);
		}

		EndDraw();

		return 0;
	}

	auto Edit::OnSetCursor(UINT, WPARAM, LPARAM) const noexcept -> Core::HandlerResult
	{
		POINT position{};
		GetCursorPos(&position);
		position = ScreenToClient(position);

		RECT rect = GetClientRect();

		auto hr = textServices->OnTxSetCursor(
			DVASPECT_CONTENT,
			0,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			&rect,
			position.x,
			position.y
		); HR_L(hr);

		return TRUE;
	}

	auto Edit::OnSetFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult
	{
		textServices->OnTxInPlaceActivate(nullptr);

		LRESULT res{ };
		textServices->TxSendMessage(msg, wParam, lParam, &res);

		return res;
	}
	auto Edit::OnKillFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult
	{
		textServices->OnTxUIDeactivate();

		LRESULT res{ };
		textServices->TxSendMessage(msg, wParam, lParam, &res);
		Invalidate();

		return res;
	}

	#pragma region ITextHost2_Impl

	HRESULT __stdcall Edit::TextHost::QueryInterface(REFIID riid, void** ppvObject)
	{
		if (!ppvObject)
		{
			return E_INVALIDARG;
		}
		*ppvObject = nullptr;

		if (riid == IID_IUnknown ||
			riid == *Msftedit::IIDITextHost ||
			riid == *Msftedit::IIDITextHost2)
		{
			AddRef();
			*ppvObject = (LPVOID)this;

			return NOERROR;
		}

		return E_NOINTERFACE;
	}
	ULONG __stdcall Edit::TextHost::AddRef()
	{
		return 1;
	}
	ULONG __stdcall Edit::TextHost::Release()
	{
		return 1;
	}

	auto Edit::TextHost::TxGetDC() -> HDC
	{
		return GetDC(parentWindow->Hwnd());
	}
	auto Edit::TextHost::TxReleaseDC(HDC hdc) -> INT
	{
		return ReleaseDC(parentWindow->Hwnd(), hdc);
	}

	auto Edit::TextHost::TxShowScrollBar(INT bars, BOOL show) -> BOOL
	{
		INT showFlag = show ? SW_SHOW : SW_HIDE;

		if (bars == SB_BOTH || bars == SB_VERT)
		{
			parentWindow->verticalScrollBar->Show(showFlag);
		}
		else if (bars == SB_BOTH || bars == SB_HORZ)
		{
			parentWindow->horizontalScrollBar->Show(showFlag);
		}

		return TRUE;
	}
	auto Edit::TextHost::TxEnableScrollBar(INT, INT) -> BOOL
	{
		//ScrollBar doesnt support enabling or disabling buttons :[
		return TRUE;
	}
	auto Edit::TextHost::TxSetScrollRange(INT bar, LONG minPos, INT maxPos, BOOL) -> BOOL
	{
		const auto& scrollBar = bar == SB_HORZ ? 
			parentWindow->horizontalScrollBar : parentWindow->verticalScrollBar;

		scrollBar->SetMinScroll(minPos);
		scrollBar->SetMaxScroll(maxPos);

		return TRUE;
	}
	auto Edit::TextHost::TxSetScrollPos(INT bar, INT pos, BOOL) -> BOOL
	{
		const auto& scrollBar = bar == SB_HORZ ? 
			parentWindow->horizontalScrollBar : parentWindow->verticalScrollBar;

		scrollBar->SetScrollPos(pos);

		return TRUE;
	}
	void Edit::TextHost::TxInvalidateRect(LPCRECT rect, BOOL erase)
	{
		InvalidateRect(parentWindow->Hwnd(), rect, erase);
	}
	void Edit::TextHost::TxViewChange(BOOL update)
	{
		if (update)
		{
			UpdateWindow(parentWindow->Hwnd());
		}
	}
	auto Edit::TextHost::TxCreateCaret(HBITMAP, INT width, INT height) -> BOOL
	{
		auto g = parentWindow->GetGraphics();

		caretRenderTarget = g.CreateCompatibleRenderTarget(
			SizeF{ static_cast<float>(width), static_cast<float>(height) },
			SizeU{ static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) });

		caretRenderTarget->BeginDraw();

		caretRenderTarget.Clear(RGBA{ 0xffffff, 1 });

		caretRenderTarget->EndDraw();

		parentWindow->Invalidate();

		return TRUE;
	}
	auto Edit::TextHost::TxShowCaret(BOOL show) -> BOOL
	{
		parentWindow->showCaret = show;

		if (show)
		{
			if (auto blinkTime = GetCaretBlinkTime(); 
				blinkTime != INFINITE)
			{
				parentWindow->caretBlinkTimerId =
					parentWindow->AddTimer(
						NULL,
						std::chrono::milliseconds{ blinkTime },
						BindMemberFunc(&Edit::CaretBlinkHandler, parentWindow)
					);
			}

			parentWindow->Invalidate();
			return TRUE;
		}

		parentWindow->RemoveTimer(parentWindow->caretBlinkTimerId);
		parentWindow->Invalidate();

		return TRUE;
	}
	auto Edit::TextHost::TxSetCaretPos(INT x, INT y) -> BOOL
	{
		caretPos.x = static_cast<int>(static_cast<float>(x) * 
			(static_cast<float>(PGUI::Core::DEFAULT_SCREEN_DPI) / 
				static_cast<float>(parentWindow->GetDPI())));
		caretPos.y = static_cast<int>(static_cast<float>(y) *
			(static_cast<float>(PGUI::Core::DEFAULT_SCREEN_DPI) /
				static_cast<float>(parentWindow->GetDPI())));;

		parentWindow->caretPositionChangedEvent.Emit();
		parentWindow->Invalidate();
		
		return TRUE;
	}
	auto Edit::TextHost::TxSetTimer(UINT idTimer, UINT timeout) -> BOOL
	{
		return static_cast<BOOL>(
			parentWindow->AddTimer(idTimer, std::chrono::milliseconds{ timeout }));
	}
	void Edit::TextHost::TxKillTimer(UINT idTimer)
	{
		parentWindow->RemoveTimer(idTimer);
	}
	void Edit::TextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT, LPCRECT, HRGN, LPRECT, UINT)
	{
		parentWindow->verticalScrollBar->ScrollRelative(dy);
		parentWindow->horizontalScrollBar->ScrollRelative(dx);
	}
	void Edit::TextHost::TxSetCapture(BOOL capture)
	{
		if (capture)
		{
			SetCapture(parentWindow->Hwnd());
		}
		else
		{
			ReleaseCapture();
		}
	}
	void Edit::TextHost::TxSetFocus()
	{
		SetFocus(parentWindow->Hwnd());
	}
	void Edit::TextHost::TxSetCursor(HCURSOR cursorHandle, BOOL)
	{
		SetCursor(cursorHandle);
	}
	auto Edit::TextHost::TxScreenToClient(LPPOINT lppt) -> BOOL
	{
		*lppt = parentWindow->ScreenToClient(*lppt);

		return TRUE;
	}
	auto Edit::TextHost::TxClientToScreen(LPPOINT lppt) -> BOOL
	{
		*lppt = parentWindow->ClientToScreen(*lppt);

		return TRUE;
	}
	auto Edit::TextHost::TxActivate(LONG* oldState) -> HRESULT
	{
		*oldState = HandleToLong(SetActiveWindow(parentWindow->Hwnd()));

		return (*oldState ? S_OK : E_FAIL);
	}
	auto Edit::TextHost::TxDeactivate(LONG newState) -> HRESULT
	{
		HWND ret = SetActiveWindow(std::bit_cast<HWND>(LongToHandle(newState)));

		return (ret ? S_OK : E_FAIL);
	}
	auto Edit::TextHost::TxGetClientRect(LPRECT prc) -> HRESULT
	{
		*prc = parentWindow->GetClientRect();

		return S_OK;
	}
	auto Edit::TextHost::TxGetViewInset(LPRECT prc) -> HRESULT
	{
		prc->top = 0;
		prc->left = 0;
		prc->bottom = 20;
		prc->right = 20;

		return S_OK;
	}
	auto Edit::TextHost::TxGetCharFormat(const CHARFORMATW** ppCF) -> HRESULT
	{
		*ppCF = &parentWindow->charFormat;

		return S_OK;
	}
	auto Edit::TextHost::TxGetParaFormat(const PARAFORMAT** ppPF) -> HRESULT
	{
		*ppPF = &parentWindow->paraFormat;

		return S_OK;
	}
	auto Edit::TextHost::TxGetSysColor(int index) -> COLORREF
	{
		return GetSysColor(index);
	}
	auto Edit::TextHost::TxGetBackStyle(TXTBACKSTYLE* pstyle) -> HRESULT
	{
		*pstyle = TXTBACK_TRANSPARENT;

		return S_OK;
	}
	auto Edit::TextHost::TxGetMaxLength(DWORD* plength) -> HRESULT
	{
		*plength = DWORD_MAX;

		return S_OK;
	}
	auto Edit::TextHost::TxGetScrollBars(DWORD* pdwScrollBar) -> HRESULT
	{
		*pdwScrollBar = ES_AUTOVSCROLL | ES_AUTOHSCROLL;

		return S_OK;
	}
	auto Edit::TextHost::TxGetPasswordChar(TCHAR* pch) -> HRESULT
	{
		*pch = parentWindow->passwordChar;

		return S_OK;
	}
	auto Edit::TextHost::TxGetAcceleratorPos(LONG* pcp) -> HRESULT
	{
		*pcp = -1;

		return S_OK;
	}
	auto Edit::TextHost::TxGetExtent(LPSIZEL lpExtent) -> HRESULT
	{
		const auto size = parentWindow->GetClientSize();
		lpExtent->cx = size.cx * 2540 / 96;
		lpExtent->cy = size.cy * 2540 / 96;

		return S_OK;
	} 
	auto Edit::TextHost::OnTxCharFormatChange(const CHARFORMATW*) -> HRESULT
	{
		return S_OK;
	}
	auto Edit::TextHost::OnTxParaFormatChange(const PARAFORMAT*) -> HRESULT
	{
		return S_OK;
	}
	auto Edit::TextHost::TxGetPropertyBits(DWORD, DWORD* pdwBits) -> HRESULT
	{
		*pdwBits = parentWindow->propertyBits;

		return S_OK;
	}
	auto Edit::TextHost::TxNotify(DWORD notifyCode, void* data) -> HRESULT
	{
		switch (notifyCode)
		{
			case EN_CHANGE:
			{
				auto changeNotify = std::bit_cast<CHANGENOTIFY*>(data);
				parentWindow->changedEvent.Emit(
					static_cast<ChangeEventType>(changeNotify->dwChangeType));
				break;
			}
			case EN_DROPFILES:
			{
				auto dropFiles = std::bit_cast<ENDROPFILES*>(data);
				parentWindow->dropFilesEvent.Emit(dropFiles->hDrop, dropFiles->cp, dropFiles->fProtected);

				break;
			}
			case EN_ERRSPACE:
			{
				parentWindow->errSpaceEvent.Emit();

				break;
			}
			case EN_LINK:
			{
				auto link = std::bit_cast<ENLINK*>(data);
				parentWindow->linkEvent.Emit(link->msg, link->wParam, link->lParam, link->chrg);

				break;
			}
			case EN_MAXTEXT:
			{
				parentWindow->maxTextEvent.Emit();

				break;
			}
			case EN_OLEOPFAILED:
			{
				auto oleOpFailed = std::bit_cast<ENOLEOPFAILED*>(data);

				parentWindow->oleOpFailedEvent.Emit(oleOpFailed->iob, oleOpFailed->lOper, oleOpFailed->hr);

				break;
			}
			case EN_PROTECTED:
			{
				auto enProtected = std::bit_cast<ENPROTECTED*>(data);

				parentWindow->protectedEvent.Emit(enProtected->msg, enProtected->wParam, enProtected->lParam, enProtected->chrg);

				break;
			}
			case EN_REQUESTRESIZE:
			{
				auto requestResize = std::bit_cast<REQRESIZE*>(data);

				parentWindow->requestResizeEvent.Emit(requestResize->rc);

				break;
			}
			case EN_SELCHANGE:
			{
				auto selChange = std::bit_cast<SELCHANGE*>(data);

				parentWindow->selectionChangeEvent.Emit(selChange->chrg, 
					static_cast<EditSelectionFlag>(selChange->seltyp));

				break;
			}
			case EN_STOPNOUNDO:
			{
				return S_OK;
			}
			case EN_UPDATE:
			{
				parentWindow->updateEvent.Emit();

				break;
			}
			case EN_CLIPFORMAT:
			{
				auto clipBoardFormat = std::bit_cast<CLIPBOARDFORMAT*>(data);

				parentWindow->clipFormatEvent.Emit(clipBoardFormat->cf);

				break;
			}
			case EN_DRAGDROPDONE:
			{
				parentWindow->dragDropDoneEvent.Emit();

				break;
			}
			case EN_LOWFIRTF:
			{
				auto lowfirtf = std::bit_cast<ENLOWFIRTF*>(data);

				parentWindow->lowFirtfEvent.Emit(lowfirtf->szControl);

				break;
			}
			case EN_OBJECTPOSITIONS:
			{
				auto objectPos = std::bit_cast<OBJECTPOSITIONS*>(data);

				std::span<long> objectPositions(objectPos->pcpPositions, objectPos->cObjectCount);

				parentWindow->objectPositionsEvent.Emit(objectPositions);

				break;
			}
			case EN_PARAGRAPHEXPANDED:
			{
				parentWindow->paragraphExpandedEvent.Emit();

				break;
			}

			case EN_HSCROLL:
			case EN_VSCROLL:
			{
				parentWindow->scrollEvent.Emit(
					notifyCode == EN_VSCROLL ? ScrollBarDirection::Vertical : ScrollBarDirection::Horizontal);

				break;
			}

			default:
				break;
		}

		return S_OK;
	}
	auto Edit::TextHost::TxImmGetContext() -> HIMC
	{
		return ImmGetContext(parentWindow->Hwnd());
	}
	void Edit::TextHost::TxImmReleaseContext(HIMC himc)
	{
		ImmReleaseContext(parentWindow->Hwnd(), himc);
	}

	auto Edit::TextHost::TxGetSelectionBarWidth(LONG* selBarWidth) -> HRESULT
	{
		*selBarWidth = 0;

		return S_OK;
	}
	auto Edit::TextHost::TxIsDoubleClickPending() -> BOOL
	{
		MSG msg;
		return PeekMessageW(&msg, nullptr, WM_LBUTTONDBLCLK - 1, WM_LBUTTONDBLCLK + 1, PM_NOREMOVE | PM_NOYIELD);
	}
	auto Edit::TextHost::TxGetWindow(HWND* phwnd) -> HRESULT
	{
		*phwnd = parentWindow->Hwnd();

		return S_OK;
	}
	auto Edit::TextHost::TxSetForegroundWindow() -> HRESULT
	{
		if (auto ret = SetForegroundWindow(parentWindow->Hwnd());
			ret)
		{
			return S_OK;
		}
		return HresultFromWin32();
	}
	auto Edit::TextHost::TxGetPalette() -> HPALETTE
	{
		return nullptr;
	}
	auto Edit::TextHost::TxGetEastAsianFlags(LONG* pFlags) -> HRESULT
	{
		*pFlags = ES_SELFIME;

		return S_OK;
	}
	auto Edit::TextHost::TxSetCursor2(HCURSOR cursorHandle, BOOL) -> HCURSOR
	{
		return SetCursor(cursorHandle);
	}
	void Edit::TextHost::TxFreeTextServicesNotification()
	{
		// E_NOTIMPL
	}
	auto Edit::TextHost::TxGetEditStyle(DWORD, DWORD* pdwData) -> HRESULT
	{
		*pdwData = 0;

		return S_OK;
	}
	auto Edit::TextHost::TxGetWindowStyles(DWORD* pdwStyle, DWORD* pdwExStyle) -> HRESULT
	{
		*pdwStyle = static_cast<DWORD>(GetWindowLongPtrW(parentWindow->Hwnd(), GWL_STYLE));
		*pdwExStyle = static_cast<DWORD>(GetWindowLongPtrW(parentWindow->Hwnd(), GWL_EXSTYLE));

		return S_OK;
	}
	auto Edit::TextHost::TxShowDropCaret(BOOL, HDC, LPCRECT) -> HRESULT
	{
		return E_NOTIMPL;
	}
	auto Edit::TextHost::TxDestroyCaret() -> HRESULT
	{
		caretRenderTarget.Reset();

		return S_OK;
	}
	auto Edit::TextHost::TxGetHorzExtent(LONG* plHorzExtent) -> HRESULT
	{
		*plHorzExtent = parentWindow->GetClientSize().cx;

		return S_OK;
	}

	#pragma endregion

	auto BuiltinFilters::NumericOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM&) noexcept -> bool
	{
		if (msg == WM_KEYDOWN || msg == WM_KEYUP)
		{
			if ((wParam >= VK_LSHIFT && wParam <= VK_MEDIA_PLAY_PAUSE) ||
				(wParam >= VK_F1 && wParam <= VK_SCROLL) ||
				(wParam >= VK_LWIN && wParam <= VK_NUMPAD9) ||
				wParam <= 0x39)
			{
				return true;
			}
			return false;
		}
		auto c = static_cast<wchar_t>(wParam);
		if ((c == L'+' || c == L'-') && edit->GetTextLength() == 0)
		{
			return true;
		}
		WORD charType = 0;
		GetStringTypeW(CT_CTYPE1, &c, 1, &charType);
		if (charType & C1_CNTRL)
		{
			return true;
		}
		if (charType & C1_DIGIT)
		{
			return true;
		}
		if (charType & C1_ALPHA && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			return true;
		}
		return false;
	}
	auto BuiltinFilters::UppercaseOnlyFilter(Core::WindowPtr<Edit>, UINT&, WPARAM& wParam, LPARAM&) noexcept -> bool
	{
		NLSVERSIONINFOEX v{ };
		v.dwNLSVersionInfoSize = sizeof(NLSVERSIONINFO);
		GetNLSVersionEx(COMPARE_STRING, LOCALE_NAME_USER_DEFAULT, &v);

		const auto inputLang = GetCurrentInputMethodLanguage();
		auto size = LCMapStringEx(inputLang.c_str(),
			LCMAP_UPPERCASE,
			std::bit_cast<LPCWSTR>(&wParam), 1,
			nullptr, 0, std::bit_cast<LPNLSVERSIONINFO>(&v), nullptr, 0);

		std::wstring str(size, L'\0');

		LCMapStringEx(inputLang.c_str(),
			LCMAP_UPPERCASE,
			std::bit_cast<LPCWSTR>(&wParam), 1,
			str.data(), size, std::bit_cast<LPNLSVERSIONINFO>(&v), nullptr, 0);

		if (size == 1)
		{
			wParam = str.at(0);
		}

		return true;
	}
	auto BuiltinFilters::LowercaseOnlyFilter(Core::WindowPtr<Edit>, UINT&, WPARAM& wParam, LPARAM&) noexcept -> bool
	{
		NLSVERSIONINFOEX v{ };
		v.dwNLSVersionInfoSize = sizeof(NLSVERSIONINFO);
		GetNLSVersionEx(COMPARE_STRING, LOCALE_NAME_USER_DEFAULT, &v);

		const auto inputLang = GetCurrentInputMethodLanguage();
		auto size = LCMapStringEx(inputLang.c_str(),
			LCMAP_LOWERCASE,
			std::bit_cast<LPCWSTR>(&wParam), 1,
			nullptr, 0, std::bit_cast<LPNLSVERSIONINFO>(&v), nullptr, 0);

		std::wstring str(size, L'\0');

		LCMapStringEx(inputLang.c_str(),
			LCMAP_LOWERCASE,
			std::bit_cast<LPCWSTR>(&wParam), 1,
			str.data(), size, std::bit_cast<LPNLSVERSIONINFO>(&v), nullptr, 0);

		if (size == 1)
		{
			wParam = str.at(0);
		}

		return true;
	}
}
