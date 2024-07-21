#include "ui/controls/Edit.hpp"

#include "core/Logger.hpp"
#include "ui/Colors.hpp"
#include "factories/WICFactory.hpp"

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
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}

		if (!createTextServices)
		{
			createTextServices = std::bit_cast<CreateTextServicesFunc>(GetProcAddress(msftedit, "CreateTextServices"));
		}
		if (createTextServices == nullptr)
		{
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}

		if (!shutdownTextServices)
		{
			shutdownTextServices = std::bit_cast<ShutdownTextServicesFunc>(GetProcAddress(msftedit, "ShutdownTextServices"));
		}
		if (shutdownTextServices == nullptr)
		{
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}
		
		if (!IIDITextServices2)
		{
			IIDITextServices2 = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextServices2"));
		}
		if (IIDITextServices2 == nullptr)
		{
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}

		if (!IIDITextHost)
		{
			IIDITextHost = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextHost"));
		}
		if (IIDITextHost == nullptr)
		{
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}

		if (!IIDITextHost2)
		{
			IIDITextHost2 = std::bit_cast<IID*>(GetProcAddress(msftedit, "IID_ITextHost2"));
		}
		if (IIDITextHost2 == nullptr)
		{
			PGUI::HR_T(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

namespace PGUI::UI::Controls
{
	long Edit::PixelsToTwips(long pixels) noexcept
	{
		return pixels * 15;
	}

	Edit::Edit(const EditParams& params) :
		Control{ Core::WindowClass::Create(L"Edit_UIControl", CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW) },
		passwordChar{ params.passwordChar }, propertyBits{ params.propertyBits | TXTBIT_D2DDWRITE | TXTBIT_D2DSUBPIXELLINES }
	{
		textHost.parentWindow = this;

		RegisterMessageHandler(WM_CREATE, &Edit::OnCreate);
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
		
		charFormat.crTextColor = RGBA{ 0xffffff };
		charFormat.yHeight = PixelsToTwips(params.fontSize);
		StringCchCopyW(charFormat.szFaceName, params.fontFace.length(), params.fontFace.c_str());

		if (UIColors::IsDarkMode())
		{
			charFormat.crTextColor = Colors::Aliceblue;
			backgroundBrush.SetParameters(0x1b1b1b);
		}
		else
		{
			charFormat.crTextColor = Colors::Black;
			backgroundBrush.SetParameters(Colors::White);
		}

		Msftedit::LoadMsftedit();
	}
	Edit::~Edit()
	{
		HRESULT hr = Msftedit::shutdownTextServices(textServices.Get()); HR_L(hr);
	}

	void Edit::SetText(std::wstring_view text) const noexcept
	{
		textServices->TxSetText(text.data());
		Invalidate();
	}

	std::wstring Edit::GetText() const noexcept
	{
		std::wstring text(GetTextLength() + 1, L'\0');

		HRESULT hr = textServices->TxSendMessage(WM_GETTEXT, text.length(),
			std::bit_cast<LPARAM>(text.data()), nullptr);
		HR_L(hr);

		return text;
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

	PointL Edit::GetCaretPosition() const noexcept
	{
		return textHost.caretPos;
	}

	std::int64_t Edit::GetCaretCharIndex() const noexcept
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

	bool Edit::CanPaste() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CANPASTE, NULL, NULL, &ret); HR_L(hr);
		
		return ret;
	}
	bool Edit::CanRedo() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CANREDO, NULL, NULL, &ret); HR_L(hr);
		
		return ret;
	}
	bool Edit::CanUndo() const noexcept
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

	std::int64_t Edit::GetCharIndexFromPosition(PointL position) const noexcept
	{
		LRESULT ret{ };
		HRESULT hr = textServices->TxSendMessage(EM_CHARFROMPOS, NULL, 
			std::bit_cast<LPARAM>(&position), &ret); HR_L(hr);

		return ret;
	}
	PointL Edit::GetPositionFromCharIndex(std::int64_t index) const noexcept
	{
		POINT p{ };

		HRESULT hr =
			textServices->TxSendMessage(EM_POSFROMCHAR, std::bit_cast<WPARAM>(p), index, nullptr);
		HR_L(hr);

		return p;
	}

	BOOL Edit::DisplayBand(RectL displayRect) const noexcept
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

	bool Edit::IsModified() const noexcept
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

	bool Edit::IsReadOnly() const noexcept
	{
		return GetWindowLongPtrW(Hwnd(), GWL_STYLE) & ES_READONLY;
	}

	void Edit::SetReadOnly(bool readonly) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETREADONLY, readonly, NULL, nullptr);
		HR_L(hr);
	}

	CharRange Edit::Find(EditFindFlag flags, std::wstring_view text, CharRange searchRange) const noexcept
	{
		FINDTEXTEXW ft{ };
		ft.chrg = searchRange;
		ft.lpstrText = text.data();

		HRESULT hr =
			textServices->TxSendMessage(EM_FINDTEXTEXW, flags, std::bit_cast<LPARAM>(&ft), nullptr);
		HR_L(hr);
		return ft.chrgText;
	}

	std::int64_t Edit::FindWordBreak(FindWordBreakOperations op, std::int64_t startPosition) const noexcept
	{
		LRESULT ret{ };
		
		HRESULT hr =
			textServices->TxSendMessage(EM_FINDWORDBREAK, 
				static_cast<WPARAM>(op), startPosition, &ret);
		HR_L(hr);

		return ret;
	}

	std::int64_t Edit::FormatRange(std::optional<FORMATRANGE> formatRange, bool display) const noexcept
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

	EditEventMaskFlag Edit::GetEventMask() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETEVENTMASK, NULL, NULL, &ret);
		HR_L(hr);

		return static_cast<EditEventMaskFlag::UnderlyingType>(ret);
	}
	void Edit::SetEventMask(EditEventMaskFlag eventFlag) const noexcept
	{
		HRESULT hr =
			textServices->TxSendMessage(EM_SETEVENTMASK, NULL, eventFlag, nullptr);
		HR_L(hr);
	}

	std::int64_t Edit::GetFirstVisibleLine() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETFIRSTVISIBLELINE, NULL, NULL, &ret);
		HR_L(hr);

		return ret;
	}

	ComPtr<IRichEditOle> Edit::GetRichEditOle() const noexcept
	{
		ComPtr<IRichEditOle> ole;

		HRESULT hr =
			textServices->TxSendMessage(EM_GETOLEINTERFACE, NULL, 
				std::bit_cast<LPARAM>(ole.GetAddressOf()), nullptr);
		HR_L(hr);
		
		return ole;
	}

	std::int64_t Edit::GetTextLimit() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETLIMITTEXT, NULL,
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	EditOptionsFlag Edit::GetOptions() const noexcept
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
				options, nullptr);
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

	RectL Edit::GetFormattingRect() const noexcept
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

	UNDONAMEID Edit::GetRedoName() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETREDONAME, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<UNDONAMEID>(ret);
	}
	UNDONAMEID Edit::GetUndoName() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_GETUNDONAME, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<UNDONAMEID>(ret);
	}

	CharRange Edit::GetSelection() const noexcept
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

	CHARFORMAT2W Edit::GetSelectionCharFormat() const noexcept
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

	EditSelectionFlag Edit::GetSelectionType() const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_SELECTIONTYPE, NULL,
				NULL, &ret);
		HR_L(hr);

		return static_cast<EditSelectionFlag>(ret);
	}

	std::wstring Edit::GetSelectedText() const noexcept
	{
		auto selectionSize = GetSelection().Length() + 1;

		std::wstring selectedText(selectionSize, L'\0');

		HRESULT hr =
			textServices->TxSendMessage(EM_GETSELTEXT, NULL,
				std::bit_cast<LPARAM>(selectedText.data()), nullptr);
		HR_L(hr);

		return selectedText;
	}

	std::int64_t Edit::GetTextLength(std::optional<GETTEXTLENGTHEX> lengthEx) const noexcept
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

	TEXTMODE Edit::GetTextMode() const noexcept
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

	std::wstring Edit::GetTextRange(CharRange charRange) const noexcept
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

	std::wstring Edit::GetLine(int line) const noexcept
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

	std::int64_t Edit::GetLineFromCharIndex(std::int64_t index) const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_EXLINEFROMCHAR, NULL,
				index, &ret);
		HR_L(hr);

		return ret;
	}

	std::int64_t Edit::GetLineIndex(std::int64_t line) const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_LINEINDEX, line,
				NULL, &ret);
		HR_L(hr);

		return ret;
	}

	std::int64_t Edit::GetLineLength(std::int64_t index) const noexcept
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

	std::int64_t Edit::StreamIn(int streamFormat, EDITSTREAM editStream) const noexcept
	{
		LRESULT ret{ };
		HRESULT hr =
			textServices->TxSendMessage(EM_STREAMIN, streamFormat,
				std::bit_cast<LPARAM>(&editStream), &ret);
		HR_L(hr);

		return ret;
	}
	std::int64_t Edit::StreamOut(int streamFormat, EDITSTREAM editStream) const noexcept
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
		auto renderer = GetRenderingInterface();

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			backgroundBrush.CreateBrush(renderer);
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

	Core::HandlerResult Edit::ForwardToTextServices(UINT msg, WPARAM wParam, LPARAM lParam) const
	{
		if (!textServices)
		{
			return DefWindowProcW(Hwnd(), msg, wParam, lParam);
		}

		LRESULT result{ };

		if (HRESULT hr = textServices->TxSendMessage(msg, wParam, lParam, &result); 
			hr == E_OUTOFMEMORY)
		{
			HR_T(hr);
		}

		return result;
	}

	Core::HandlerResult Edit::OnCreate(UINT, WPARAM, LPARAM lParam)
	{
		const auto clientRect = GetClientRect();
		const auto clientSize = clientRect.Size();

		auto params = ScrollBar::ScrollBarParams{ 5, 1, 5, 0 };
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
		HRESULT hr = Msftedit::createTextServices(nullptr, &textHost, unk.GetAddressOf()); HR_T(hr);
		hr = unk->QueryInterface(*Msftedit::IIDITextServices2, 
			std::bit_cast<void**>(textServices.GetAddressOf())); HR_T(hr);

		textServices->TxSendMessage(EM_SETTYPOGRAPHYOPTIONS,
			0x1000 | 0x2000,
			0x1000 | 0x2000, nullptr);
		textServices->OnTxInPlaceActivate(nullptr);

		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		textServices->TxSetText(createStruct->lpszName);

		return 0;
	}

	Core::HandlerResult Edit::OnPaint(UINT, WPARAM, LPARAM) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		auto clientRect = GetClientRect();
		renderer->FillRectangle(clientRect, backgroundBrush->GetBrushPtr());

		RECT bounds = clientRect;
		bounds.right -= 20;
		bounds.bottom -= 20;

		textServices->TxDrawD2D(renderer.Get(), std::bit_cast<LPRECTL>(&bounds), nullptr, TXTVIEW_ACTIVE);

		if (textHost.caretRenderTarget && showCaret)
		{
			ComPtr<ID2D1Bitmap> bmp;
			textHost.caretRenderTarget->GetBitmap(&bmp);

			renderer->DrawImage(bmp.Get(), textHost.caretPos, RectF{ {}, bmp->GetSize() },
				D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_MASK_INVERT);
		}

		EndDraw();

		return 0;
	}

	Core::HandlerResult Edit::OnSetCursor(UINT, WPARAM, LPARAM) const noexcept
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

	Core::HandlerResult Edit::OnSetFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
	{
		textServices->OnTxInPlaceActivate(nullptr);

		LRESULT res{ };
		textServices->TxSendMessage(msg, wParam, lParam, &res);

		return res;
	}
	Core::HandlerResult Edit::OnKillFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
	{
		textServices->OnTxInPlaceDeactivate();

		LRESULT res{ };
		textServices->TxSendMessage(msg, wParam, lParam, &res);

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
	ULONG __stdcall Edit::TextHost::AddRef(void)
	{
		return 1;
	}
	ULONG __stdcall Edit::TextHost::Release(void)
	{
		return 1;
	}

	HDC Edit::TextHost::TxGetDC()
	{
		return GetDC(parentWindow->Hwnd());
	}
	INT Edit::TextHost::TxReleaseDC(HDC hdc)
	{
		return ReleaseDC(parentWindow->Hwnd(), hdc);
	}

	BOOL Edit::TextHost::TxShowScrollBar(INT bars, BOOL show)
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
	BOOL Edit::TextHost::TxEnableScrollBar(INT, INT)
	{
		//ScrollBar doesnt support enabling or disabling buttons :[
		return TRUE;
	}
	BOOL Edit::TextHost::TxSetScrollRange(INT bar, LONG minPos, INT maxPos, BOOL)
	{
		const auto& scrollBar = bar == SB_HORZ ? 
			parentWindow->horizontalScrollBar : parentWindow->verticalScrollBar;

		scrollBar->SetMinScroll(minPos);
		scrollBar->SetMaxScroll(maxPos);

		return TRUE;
	}
	BOOL Edit::TextHost::TxSetScrollPos(INT bar, INT pos, BOOL)
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
	BOOL Edit::TextHost::TxCreateCaret(HBITMAP, INT width, INT height)
	{
		auto renderer = parentWindow->GetRenderingInterface();

		HRESULT hr = renderer->CreateCompatibleRenderTarget(
			SizeI{ width, height }, &caretRenderTarget); HR_L(hr);

		caretRenderTarget->BeginDraw();

		caretRenderTarget->Clear(RGBA{ 0xffffff, 1 });

		caretRenderTarget->EndDraw();

		parentWindow->Invalidate();

		return SUCCEEDED(hr) ? TRUE : FALSE;
	}
	BOOL Edit::TextHost::TxShowCaret(BOOL show)
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
	BOOL Edit::TextHost::TxSetCaretPos(INT x, INT y)
	{
		caretPos.x = x;
		caretPos.y = y;

		parentWindow->caretPositionChangedEvent.Emit();
		parentWindow->Invalidate();
		
		return TRUE;
	}
	BOOL Edit::TextHost::TxSetTimer(UINT idTimer, UINT timeout)
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
	BOOL Edit::TextHost::TxScreenToClient(LPPOINT lppt)
	{
		*lppt = parentWindow->ScreenToClient(*lppt);

		return TRUE;
	}
	BOOL Edit::TextHost::TxClientToScreen(LPPOINT lppt)
	{
		*lppt = parentWindow->ClientToScreen(*lppt);

		return TRUE;
	}
	HRESULT Edit::TextHost::TxActivate(LONG* oldState)
	{
		*oldState = HandleToLong(SetActiveWindow(parentWindow->Hwnd()));

		return (*oldState ? S_OK : E_FAIL);
	}
	HRESULT Edit::TextHost::TxDeactivate(LONG newState)
	{
		HWND ret = SetActiveWindow(std::bit_cast<HWND>(LongToHandle(newState)));

		return (ret ? S_OK : E_FAIL);
	}
	HRESULT Edit::TextHost::TxGetClientRect(LPRECT prc)
	{
		*prc = parentWindow->GetClientRect();

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetViewInset(LPRECT prc)
	{
		prc->top = 0;
		prc->left = 0;
		prc->bottom = 20;
		prc->right = 20;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetCharFormat(const CHARFORMATW** ppCF)
	{
		*ppCF = &parentWindow->charFormat;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetParaFormat(const PARAFORMAT** ppPF)
	{
		*ppPF = &parentWindow->paraFormat;

		return S_OK;
	}
	COLORREF Edit::TextHost::TxGetSysColor(int index)
	{
		return GetSysColor(index);
	}
	HRESULT Edit::TextHost::TxGetBackStyle(TXTBACKSTYLE* pstyle)
	{
		*pstyle = TXTBACK_TRANSPARENT;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetMaxLength(DWORD* plength)
	{
		*plength = DWORD_MAX;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetScrollBars(DWORD* pdwScrollBar)
	{
		*pdwScrollBar = ES_AUTOVSCROLL | ES_AUTOHSCROLL;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetPasswordChar(TCHAR* pch)
	{
		*pch = parentWindow->passwordChar;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetAcceleratorPos(LONG* pcp)
	{
		*pcp = -1;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetExtent(LPSIZEL lpExtent)
	{
		const auto size = parentWindow->GetClientSize();
		lpExtent->cx = size.cx * 2540 / 96;
		lpExtent->cy = size.cy * 2540 / 96;

		return S_OK;
	} 
	HRESULT Edit::TextHost::OnTxCharFormatChange(const CHARFORMATW*)
	{
		return S_OK;
	}
	HRESULT Edit::TextHost::OnTxParaFormatChange(const PARAFORMAT*)
	{
		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetPropertyBits(DWORD, DWORD* pdwBits)
	{
		*pdwBits = parentWindow->propertyBits;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxNotify(DWORD notifyCode, void* data)
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

				parentWindow->selectionChangeEvent.Emit(selChange->chrg, selChange->seltyp);

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
	HIMC Edit::TextHost::TxImmGetContext()
	{
		return ImmGetContext(parentWindow->Hwnd());
	}
	void Edit::TextHost::TxImmReleaseContext(HIMC himc)
	{
		ImmReleaseContext(parentWindow->Hwnd(), himc);
	}

	HRESULT Edit::TextHost::TxGetSelectionBarWidth(LONG* selBarWidth)
	{
		*selBarWidth = 0;

		return S_OK;
	}
	BOOL Edit::TextHost::TxIsDoubleClickPending()
	{
		MSG msg;
		return PeekMessageW(&msg, nullptr, WM_LBUTTONDBLCLK - 1, WM_LBUTTONDBLCLK + 1, PM_NOREMOVE | PM_NOYIELD);
	}
	HRESULT Edit::TextHost::TxGetWindow(HWND* phwnd)
	{
		*phwnd = parentWindow->Hwnd();

		return S_OK;
	}
	HRESULT Edit::TextHost::TxSetForegroundWindow()
	{
		if (auto ret = SetForegroundWindow(parentWindow->Hwnd());
			ret)
		{
			return S_OK;
		}
		return HRESULT_FROM_WIN32(GetLastError());
	}
	HPALETTE Edit::TextHost::TxGetPalette()
	{
		return nullptr;
	}
	HRESULT Edit::TextHost::TxGetEastAsianFlags(LONG* pFlags)
	{
		*pFlags = ES_SELFIME;

		return S_OK;
	}
	HCURSOR Edit::TextHost::TxSetCursor2(HCURSOR cursorHandle, BOOL)
	{
		return SetCursor(cursorHandle);
	}
	void Edit::TextHost::TxFreeTextServicesNotification()
	{
		// E_NOTIMPL
	}
	HRESULT Edit::TextHost::TxGetEditStyle(DWORD, DWORD* pdwData)
	{
		*pdwData = 0;

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetWindowStyles(DWORD* pdwStyle, DWORD* pdwExStyle)
	{
		*pdwStyle = static_cast<DWORD>(GetWindowLongPtrW(parentWindow->Hwnd(), GWL_STYLE));
		*pdwExStyle = static_cast<DWORD>(GetWindowLongPtrW(parentWindow->Hwnd(), GWL_EXSTYLE));

		return S_OK;
	}
	HRESULT Edit::TextHost::TxShowDropCaret(BOOL, HDC, LPCRECT)
	{
		return E_NOTIMPL;
	}
	HRESULT Edit::TextHost::TxDestroyCaret()
	{
		caretRenderTarget.Reset();

		return S_OK;
	}
	HRESULT Edit::TextHost::TxGetHorzExtent(LONG* plHorzExtent)
	{
		*plHorzExtent = parentWindow->GetClientSize().cx;

		return S_OK;
	}

	#pragma endregion
}
