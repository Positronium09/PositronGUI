#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/controls/ScrollBar.hpp"
#include "ui/Brush.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"
#include "ui/bmp/Bitmap.hpp"
#include "helpers/ComPtr.hpp"
#include "helpers/EnumFlag.hpp"
#include "graphics/BitmapRenderTarget.hpp"

#include <functional>
#include <CommCtrl.h>
#include <Richedit.h>
#include <RichOle.h>
#include <TextServ.h>


namespace PGUI::UI::Controls
{
	enum class FindWordBreakOperations
	{
		Classify = WB_CLASSIFY,
		IsDelimeter = WB_ISDELIMITER,
		Left = WB_LEFT,
		LeftBreak = WB_LEFTBREAK,
		MoveWordLeft = WB_MOVEWORDLEFT,
		Right = WB_RIGHT,
		RightBreak = WB_RIGHTBREAK,
		MoveWordRight = WB_MOVEWORDRIGHT,
	};

	enum class ChangeEventType
	{
		Generic = CN_GENERIC,
		NewRedo = CN_NEWREDO,
		NewUndo = CN_NEWUNDO,
		TextChanged = CN_TEXTCHANGED
	};

	enum class EditFindFlag : std::uint64_t
	{
		Down = 0x1, // FR_DOWN
		WholeWord = 0x2, // FR_WHOLEWORD
		CaseSensitive = 0x4, // FR_MATCHCASE
		MatchCase = CaseSensitive,
		MatchLeftHamza = FR_MATCHALEFHAMZA,
		MatchDiac = FR_MATCHDIAC,
		MatchKashida = FR_MATCHKASHIDA
	};

	enum class EditSelectionFlag
	{
		Text = SEL_TEXT,
		Object = SEL_OBJECT,
		MultiChar = SEL_MULTICHAR,
		MultiObject = SEL_MULTIOBJECT
	};

	enum class EditOptionsFlag
	{
		AutoWordSelection = ECO_AUTOWORDSELECTION,
		AutoVscroll = ECO_AUTOVSCROLL,
		AutoHscroll = ECO_AUTOHSCROLL,
		NoHidesel = ECO_NOHIDESEL,
		Readonly = ECO_READONLY,
		WantReturn = ECO_WANTRETURN,
		Selectionbar = ECO_SELECTIONBAR,
		Vertical = ECO_VERTICAL
	};

	enum class EditEventMaskFlag
	{
		None = ENM_NONE,
		Change = ENM_CHANGE,
		Clipformat = ENM_CLIPFORMAT,
		DragdropDone = ENM_DRAGDROPDONE,
		DropFiles = ENM_DROPFILES,
		Link = ENM_LINK,
		LowFirtf = ENM_LOWFIRTF,
		ObjectPositions = ENM_OBJECTPOSITIONS,
		ParagraphExpanded = ENM_PARAGRAPHEXPANDED,
		Protected = ENM_PROTECTED,
		RequestResize = ENM_REQUESTRESIZE,
		Scroll = ENM_SCROLL,
		SelectionChange = ENM_SELCHANGE,
		Update = ENM_UPDATE
	};
}
EnableEnumFlag(PGUI::UI::Controls::EditFindFlag)
EnableEnumFlag(PGUI::UI::Controls::EditSelectionFlag)
EnableEnumFlag(PGUI::UI::Controls::EditOptionsFlag)
EnableEnumFlag(PGUI::UI::Controls::EditEventMaskFlag)

namespace PGUI::UI::Controls
{
	struct CharRange
	{
		long min = 0;
		long max = 0;

		CharRange() noexcept = default;
		explicit(false) CharRange(CHARRANGE charRange) noexcept :
			min{ charRange.cpMin }, max{ charRange.cpMax }
		{
		}
		CharRange(long min, long max) noexcept :
			min{ min }, max{ max }
		{
		}

		explicit(false) operator CHARRANGE() const noexcept
		{
			CHARRANGE charRange{ };
			charRange.cpMin = min;
			charRange.cpMax = max;
			return charRange;
		}

		long Length() const noexcept { return max - min + 1; }
	};

	struct FindTextStruct
	{
		CharRange toSearch{ 0, 1 };
		std::wstring text;
	};


	class Edit : public Control
	{
		class TextHost : public ITextHost2
		{
			friend class Edit;

			public:
			virtual ~TextHost() = default;

			#pragma region ITextHost2_Impl

			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
			ULONG STDMETHODCALLTYPE AddRef(void) override;
			ULONG STDMETHODCALLTYPE Release(void) override;

			HDC TxGetDC() override;
			INT TxReleaseDC(HDC hdc) override;
			BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override;
			BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override;
			BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override;
			BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override;
			void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;
			void TxViewChange(BOOL fUpdate) override;
			BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override;
			BOOL TxShowCaret(BOOL fShow) override;
			BOOL TxSetCaretPos(INT x, INT y) override;
			BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override;
			void TxKillTimer(UINT idTimer) override;
			void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) override;
			void TxSetCapture(BOOL fCapture) override;
			void TxSetFocus() override;
			void TxSetCursor(HCURSOR hcur, BOOL fText) override;
			BOOL TxScreenToClient(LPPOINT lppt) override;
			BOOL TxClientToScreen(LPPOINT lppt) override;
			HRESULT TxActivate(LONG* plOldState) override;
			HRESULT TxDeactivate(LONG lNewState) override;
			HRESULT TxGetClientRect(LPRECT prc) override;
			HRESULT TxGetViewInset(LPRECT prc) override;
			HRESULT TxGetCharFormat(const CHARFORMATW** ppCF) override;
			HRESULT TxGetParaFormat(const PARAFORMAT** ppPF) override;
			COLORREF TxGetSysColor(int nIndex) override;
			HRESULT TxGetBackStyle(TXTBACKSTYLE* pstyle) override;
			HRESULT TxGetMaxLength(DWORD* plength) override;
			HRESULT TxGetScrollBars(DWORD* pdwScrollBar) override;
			HRESULT TxGetPasswordChar(TCHAR* pch) override;
			HRESULT TxGetAcceleratorPos(LONG* pcp) override;
			HRESULT TxGetExtent(LPSIZEL lpExtent) override;
			HRESULT OnTxCharFormatChange(const CHARFORMATW* pCF) override;
			HRESULT OnTxParaFormatChange(const PARAFORMAT* pPF) override;
			HRESULT TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits) override;
			HRESULT TxNotify(DWORD iNotify, void* pv) override;
			HIMC TxImmGetContext() override;
			void TxImmReleaseContext(HIMC himc) override;

			HRESULT TxGetSelectionBarWidth(LONG* lSelBarWidth) override;
			BOOL TxIsDoubleClickPending() override;
			HRESULT TxGetWindow(HWND* phwnd) override;
			HRESULT TxSetForegroundWindow() override;
			HPALETTE TxGetPalette() override;
			HRESULT TxGetEastAsianFlags(LONG* pFlags) override;
			HCURSOR TxSetCursor2(HCURSOR hcur, BOOL bText) override;
			void TxFreeTextServicesNotification() override;
			HRESULT TxGetEditStyle(DWORD dwItem, DWORD* pdwData) override;
			HRESULT TxGetWindowStyles(DWORD* pdwStyle, DWORD* pdwExStyle) override;
			HRESULT TxShowDropCaret(BOOL fShow, HDC hdc, LPCRECT prc) override;
			HRESULT TxDestroyCaret() override;
			HRESULT TxGetHorzExtent(LONG* plHorzExtent) override;

			#pragma endregion

			private:
			PointL caretPos;
			Graphics::BitmapRenderTarget caretRenderTarget;
			Edit* parentWindow = nullptr;
		};

		public:
		/**
		 * @return true if key event should be processed false otherwise
		 */
		using KeyFilterFunction = std::function<bool(Edit*, UINT&, WPARAM&, LPARAM&)>;

		struct EditParams
		{
			long fontSize;
			std::wstring fontFace;
			wchar_t passwordChar = L'*';
			DWORD propertyBits;

			EditParams(
				long fontSize = 24, 
				bool singleLine = false,
				std::wstring_view fontFace = L"Segoe UI", 
				wchar_t passwordChar = L'*', 
				DWORD propertyBits = TXTBIT_RICHTEXT | TXTBIT_SAVESELECTION) noexcept :
				fontSize{ fontSize }, fontFace{ fontFace }, 
				passwordChar{ passwordChar }, 
				propertyBits{ propertyBits | (singleLine ? 0 : TXTBIT_MULTILINE) }
			{
			}
		};

		static long PixelsToTwips(long pixels) noexcept;
		
		explicit Edit(const EditParams& params = EditParams{ });

		void SetText(std::wstring_view text) const noexcept;
		[[nodiscard]] std::wstring GetText() const noexcept;

		void SetKeyFilter(const KeyFilterFunction& keyFilter) noexcept;
		void RemoveKeyFilter() noexcept;

		void SetTextColor(RGBA color) noexcept;
		void SetFontSize(long fontSize) noexcept;
		void SetFontFace(std::wstring_view fontFace) noexcept;

		/**
		 * @return Caret position in client coordiantes
		 */
		PointL GetCaretPosition() const noexcept;
		std::int64_t GetCaretCharIndex() const noexcept;

		#pragma region RICH_EDIT_IMPL

		void SetPasswordChar(wchar_t passChar) noexcept;
		[[nodiscard]] wchar_t GetPasswordChar() const noexcept { return passwordChar; }

		void SetBackgroundBrush(const Brush& bkgndBrush) noexcept;
		[[nodiscard]] const Brush& GetTextBrush() const noexcept { return backgroundBrush; }

		[[nodiscard]] bool CanPaste() const noexcept;
		[[nodiscard]] bool CanRedo() const noexcept;
		[[nodiscard]] bool CanUndo() const noexcept;

		void Clear() const noexcept;
		void Copy() const noexcept;
		void Cut() const noexcept;
		void Paste() const noexcept;
		/**
		 * @brief See https://learn.microsoft.com/en-us/windows/win32/Controls/em-pastespecial for more info 
		 */
		void PasteSpecial(UINT clipFormat, DWORD aspect = 0, HMETAFILE metaFile = nullptr) const noexcept;

		void SetUndoLimit(std::int64_t undoLimit) const noexcept;

		/**
		 * @param[in] position - Position in client coordiantes
		 * @return 0 based index of the closest character
		 */
		[[nodiscard]] std::int64_t GetCharIndexFromPosition(PointL position) const noexcept;
		/**
		 * @param index - 0 based index of the character
		 * @return Position of top left corner of the character in client coordinates
		 */
		[[nodiscard]] PointL GetPositionFromCharIndex(std::int64_t index) const noexcept;

		BOOL DisplayBand(RectL displayRect) const noexcept;

		void EmptyUndoBuffer() const noexcept;

		[[nodiscard]] bool IsModified() const noexcept;
		void SetModified(bool modified = true) const noexcept;

		[[nodiscard]] bool IsReadOnly() const noexcept;
		void SetReadOnly(bool readonly = true) noexcept;

		[[nodiscard]] bool IsPassword() const noexcept;
		void SetPassword(bool password = true) noexcept;

		/**
		 * @param flags - Specifies the behavior of the search operation (https://learn.microsoft.com/en-us/windows/win32/Controls/em-findtextex)
		 * @param text - Text to be found
		 * @return CharRange of the text in which it was found, not found if both min and max are -1
		 */
		[[nodiscard]] CharRange Find(EditFindFlag flags, std::wstring_view text, CharRange searchRange = CharRange{ 0, -1 }) const noexcept;
		[[nodiscard]] std::int64_t FindWordBreak(FindWordBreakOperations op, std::int64_t startPosition) const noexcept;

		/**
		 * @param formatRange - Information about the output device
		 * If given std::nullopt cached information within the rich edit control can be freed
		 * @param display - Indicates if the text should be rendered. If false the text is just measured
		 * @return The index of the last character that fits in the region plus 1
		 */
		[[nodiscard]] std::int64_t FormatRange(std::optional<FORMATRANGE> formatRange, bool display = true) const noexcept;

		[[nodiscard]] CHARFORMAT2W GetDefaultCharFormat() const noexcept { return charFormat; }
		void SetDefaultCharFormat(const CHARFORMAT2W& charFormat) noexcept;

		[[nodiscard]] EditEventMaskFlag GetEventMask() const noexcept;
		void SetEventMask(EditEventMaskFlag eventFlags) const noexcept;

		[[nodiscard]] std::int64_t GetFirstVisibleLine() const noexcept;

		[[nodiscard]] std::int64_t GetTextLimit() const noexcept;
		void SetTextLimit(std::int64_t limit) const noexcept;

		[[nodiscard]] std::wstring GetLine(int line) const noexcept;

		[[nodiscard]] EditOptionsFlag GetOptions()  const noexcept;
		void SetOptions(EditOptionsFlag options) const noexcept;

		[[nodiscard]] PARAFORMAT2 GetParaFormat() const noexcept { return paraFormat; }
		void SetParaFormat(const PARAFORMAT2& paraFormat) noexcept;

		[[nodiscard]] RectL GetFormattingRect() const noexcept;
		void SetFormattingRect(RectL rect) const noexcept;

		[[nodiscard]] UNDONAMEID GetRedoName() const noexcept;
		[[nodiscard]] UNDONAMEID GetUndoName() const noexcept;

		/**
		 * @return first is beginning and the last is ending character
		 * If start is 0 and end is -1 all text is selected
		 */
		[[nodiscard]] CharRange GetSelection() const noexcept;
		void SetSelection(CharRange charRange) const noexcept;

		[[nodiscard]] CHARFORMAT2W GetSelectionCharFormat() const noexcept;
		void SetSelectionCharFormat(const CHARFORMAT2W& charFormat) const noexcept;

		void SetWordCharFormat(const CHARFORMAT2W& charFormat) const noexcept;

		[[nodiscard]] EditSelectionFlag GetSelectionType() const noexcept;

		[[nodiscard]] std::wstring GetSelectedText() const noexcept;

		[[nodiscard]] std::int64_t GetTextLength(
			std::optional<GETTEXTLENGTHEX> lengthEx = std::nullopt) const noexcept;

		[[nodiscard]] TEXTMODE GetTextMode() const noexcept;
		void SetTextMode(TEXTMODE textMode) const noexcept;

		[[nodiscard]] std::wstring GetTextRange(CharRange charRange) const noexcept;

		void SetTargetDevice(HDC hdc, std::int64_t lineWidth) const noexcept;

		void ShowSelection() const noexcept;
		void HideSelection() const noexcept;
		void ReplaceSelection(std::wstring_view newText, bool canUndo = false) const noexcept;

		void StopGroupTyping() const noexcept;

		/**
		 * @param index - 0 based index of the character 
		 * If index is -1 then it specifies the current line (the line that contains the caret)
		 * @return 0 based line number of the line containing the character index
		 */
		[[nodiscard]] std::int64_t GetLineFromCharIndex(std::int64_t index = -1) const noexcept;
		/**
		 * @param line - 0 based line index 
		 * If line is -1 it specifies the current line number (the line that contains the caret)
		 * @return Character index of the line 
		 * If it is -1 the specified line number is greater than the number of lines in the edit control
		 */
		[[nodiscard]] std::int64_t GetLineIndex(std::int64_t line = -1) const noexcept;
		/**
		 * @param line - 0 based index of the character in the line
		 * If it is -1 returns the number of unselected characters on lines containing selected characters
		 * Look at https://learn.microsoft.com/en-us/windows/win32/Controls/em-linelength for more info
		 * @return Number of characters in the line
		 */
		[[nodiscard]] std::int64_t GetLineLength(std::int64_t index = -1) const noexcept;

		void ScrollLines(std::int64_t lines) const noexcept;
		
		void RequestSize() const noexcept;

		[[nodiscard]] ComPtr<IRichEditOle> GetRichEditOle() const noexcept;
		void SetOleCallack(ComPtr<IRichEditOleCallback> callback) const noexcept;

		void EnableAutoURLDetect() const noexcept;
		void DisableAutoURLDetect() const noexcept;

		std::int64_t StreamIn(int streamFormat, EDITSTREAM editStream) const noexcept;
		std::int64_t StreamOut(int streamFormat, EDITSTREAM editStream) const noexcept;

		#pragma endregion

		#pragma region Events

		auto& ChangedEvent() { return changedEvent; }
		auto& ClipFormatEvent() { return clipFormatEvent; }
		auto& DragDropDoneEvent() { return dragDropDoneEvent; }
		auto& DropFilesEvent() { return dropFilesEvent; }
		auto& LinkEvent() { return linkEvent; }
		auto& LowFirtfEvent() { return lowFirtfEvent; }
		auto& ObjectPositionsEvent() { return objectPositionsEvent; }
		auto& OleOpFailedEvent() { return oleOpFailedEvent; }
		auto& ParagraphExpandedEvent() { return paragraphExpandedEvent; }
		auto& ProtectedEvent() { return protectedEvent; }
		auto& RequestResizeEvent() { return requestResizeEvent; }
		auto& ScrollEvent() { return scrollEvent; }
		auto& SelectionChangeEvent() { return selectionChangeEvent; }
		auto& UpdateEvent() { return updateEvent; }
		auto& ErrSpaceEvent() { return errSpaceEvent; }
		auto& MaxTextEvent() { return maxTextEvent; }
		auto& CaretPositionChangedEvent() { return caretPositionChangedEvent; }

		#pragma endregion

		private:
		TextHost textHost;

		Brush backgroundBrush;

		KeyFilterFunction filteringFunction;

		ComPtr<ITextServices2> textServices;
		Core::WindowPtr<ScrollBar> verticalScrollBar{};
		Core::WindowPtr<ScrollBar> horizontalScrollBar{};

		#pragma region Events

		Core::Event<ChangeEventType> changedEvent;
		Core::Event<CLIPFORMAT> clipFormatEvent;
		Core::Event<> dragDropDoneEvent;
		Core::Event<HANDLE, long, bool> dropFilesEvent;
		Core::Event<UINT, WPARAM, LPARAM, CharRange> linkEvent;
		Core::Event<std::string_view> lowFirtfEvent;
		Core::Event<std::span<long>> objectPositionsEvent;
		Core::Event<long, long, HRESULT> oleOpFailedEvent;
		Core::Event<> paragraphExpandedEvent;
		Core::Event<UINT, WPARAM, LPARAM, CharRange> protectedEvent;
		Core::Event<RectL> requestResizeEvent;
		Core::Event<ScrollBarDirection> scrollEvent;
		Core::Event<CharRange, EditSelectionFlag> selectionChangeEvent;
		Core::Event<> updateEvent;
		Core::Event<> errSpaceEvent;
		Core::Event<> maxTextEvent;
		Core::Event<> caretPositionChangedEvent;

		#pragma endregion

		wchar_t passwordChar;
		DWORD propertyBits;
		CHARFORMAT2W charFormat{ };
		PARAFORMAT2 paraFormat{ };

		bool showCaret = false;

		Core::TimerId caretBlinkTimerId{};

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		void CaretBlinkHandler(Core::TimerId timerId);

		Core::HandlerResult OnDPIChange(float dpiScale, RectI suggestedRect) override;
		Core::HandlerResult ForwardToTextServices(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSetCursor(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnSetFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnKillFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};

	namespace BuiltinFilters
	{
		bool NumericOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept;
		bool UppercaseOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept;
		bool LowercaseOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept;
	}
}
