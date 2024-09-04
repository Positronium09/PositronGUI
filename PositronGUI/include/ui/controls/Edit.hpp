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

		[[nodiscard]] auto Length() const noexcept -> long { return max - min + 1; }
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
			ULONG STDMETHODCALLTYPE AddRef() override;
			ULONG STDMETHODCALLTYPE Release() override;

			auto TxGetDC() -> HDC override;
			auto TxReleaseDC(HDC hdc) -> INT override;
			auto TxShowScrollBar(INT fnBar, BOOL fShow) -> BOOL override;
			auto TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) -> BOOL override;
			auto TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) -> BOOL override;
			auto TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) -> BOOL override;
			void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;
			void TxViewChange(BOOL fUpdate) override;
			auto TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) -> BOOL override;
			auto TxShowCaret(BOOL fShow) -> BOOL override;
			auto TxSetCaretPos(INT x, INT y) -> BOOL override;
			auto TxSetTimer(UINT idTimer, UINT uTimeout) -> BOOL override;
			void TxKillTimer(UINT idTimer) override;
			void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) override;
			void TxSetCapture(BOOL fCapture) override;
			void TxSetFocus() override;
			void TxSetCursor(HCURSOR hcur, BOOL fText) override;
			auto TxScreenToClient(LPPOINT lppt) -> BOOL override;
			auto TxClientToScreen(LPPOINT lppt) -> BOOL override;
			auto TxActivate(LONG* plOldState) -> HRESULT override;
			auto TxDeactivate(LONG lNewState) -> HRESULT override;
			auto TxGetClientRect(LPRECT prc) -> HRESULT override;
			auto TxGetViewInset(LPRECT prc) -> HRESULT override;
			auto TxGetCharFormat(const CHARFORMATW** ppCF) -> HRESULT override;
			auto TxGetParaFormat(const PARAFORMAT** ppPF) -> HRESULT override;
			auto TxGetSysColor(int nIndex) -> COLORREF override;
			auto TxGetBackStyle(TXTBACKSTYLE* pstyle) -> HRESULT override;
			auto TxGetMaxLength(DWORD* plength) -> HRESULT override;
			auto TxGetScrollBars(DWORD* pdwScrollBar) -> HRESULT override;
			auto TxGetPasswordChar(TCHAR* pch) -> HRESULT override;
			auto TxGetAcceleratorPos(LONG* pcp) -> HRESULT override;
			auto TxGetExtent(LPSIZEL lpExtent) -> HRESULT override;
			auto OnTxCharFormatChange(const CHARFORMATW* pCF) -> HRESULT override;
			auto OnTxParaFormatChange(const PARAFORMAT* pPF) -> HRESULT override;
			auto TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits) -> HRESULT override;
			auto TxNotify(DWORD iNotify, void* pv) -> HRESULT override;
			auto TxImmGetContext() -> HIMC override;
			void TxImmReleaseContext(HIMC himc) override;

			auto TxGetSelectionBarWidth(LONG* lSelBarWidth) -> HRESULT override;
			auto TxIsDoubleClickPending() -> BOOL override;
			auto TxGetWindow(HWND* phwnd) -> HRESULT override;
			auto TxSetForegroundWindow() -> HRESULT override;
			auto TxGetPalette() -> HPALETTE override;
			auto TxGetEastAsianFlags(LONG* pFlags) -> HRESULT override;
			auto TxSetCursor2(HCURSOR hcur, BOOL bText) -> HCURSOR override;
			void TxFreeTextServicesNotification() override;
			auto TxGetEditStyle(DWORD dwItem, DWORD* pdwData) -> HRESULT override;
			auto TxGetWindowStyles(DWORD* pdwStyle, DWORD* pdwExStyle) -> HRESULT override;
			auto TxShowDropCaret(BOOL fShow, HDC hdc, LPCRECT prc) -> HRESULT override;
			auto TxDestroyCaret() -> HRESULT override;
			auto TxGetHorzExtent(LONG* plHorzExtent) -> HRESULT override;

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

		static auto PixelsToTwips(long pixels) noexcept -> long;
		
		explicit Edit(const EditParams& params = EditParams{ });

		void SetText(std::wstring_view text) const noexcept;
		[[nodiscard]] auto GetText() const noexcept -> std::wstring;

		void SetKeyFilter(const KeyFilterFunction& keyFilter) noexcept;
		void RemoveKeyFilter() noexcept;

		void SetTextColor(RGBA color) noexcept;
		void SetFontSize(long fontSize) noexcept;
		void SetFontFace(std::wstring_view fontFace) noexcept;

		/**
		 * @return Caret position in client coordiantes
		 */
		[[nodiscard]] auto GetCaretPosition() const noexcept -> PointL;
		[[nodiscard]] auto GetCaretCharIndex() const noexcept -> std::int64_t;

		#pragma region RICH_EDIT_IMPL

		void SetPasswordChar(wchar_t passChar) noexcept;
		[[nodiscard]] auto GetPasswordChar() const noexcept -> wchar_t { return passwordChar; }

		void SetBackgroundBrush(const Brush& bkgndBrush) noexcept;
		[[nodiscard]] auto GetTextBrush() const noexcept -> const Brush& { return backgroundBrush; }

		[[nodiscard]] auto CanPaste() const noexcept -> bool;
		[[nodiscard]] auto CanRedo() const noexcept -> bool;
		[[nodiscard]] auto CanUndo() const noexcept -> bool;

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
		[[nodiscard]] auto GetCharIndexFromPosition(PointL position) const noexcept -> std::int64_t;
		/**
		 * @param index - 0 based index of the character
		 * @return Position of top left corner of the character in client coordinates
		 */
		[[nodiscard]] auto GetPositionFromCharIndex(std::int64_t index) const noexcept -> PointL;

		[[nodiscard]] auto DisplayBand(RectL displayRect) const noexcept -> BOOL;

		void EmptyUndoBuffer() const noexcept;

		[[nodiscard]] auto IsModified() const noexcept -> bool;
		void SetModified(bool modified = true) const noexcept;

		[[nodiscard]] auto IsReadOnly() const noexcept -> bool;
		void SetReadOnly(bool readonly = true) noexcept;

		[[nodiscard]] auto IsPassword() const noexcept -> bool;
		void SetPassword(bool password = true) noexcept;

		/**
		 * @param flags - Specifies the behavior of the search operation (https://learn.microsoft.com/en-us/windows/win32/Controls/em-findtextex)
		 * @param text - Text to be found
		 * @return CharRange of the text in which it was found, not found if both min and max are -1
		 */
		[[nodiscard]] auto Find(EditFindFlag flags, std::wstring_view text, CharRange searchRange = CharRange{ 0, -1 }) const noexcept -> CharRange;
		[[nodiscard]] auto FindWordBreak(FindWordBreakOperations op, std::int64_t startPosition) const noexcept -> std::int64_t;

		/**
		 * @param formatRange - Information about the output device
		 * If given std::nullopt cached information within the rich edit control can be freed
		 * @param display - Indicates if the text should be rendered. If false the text is just measured
		 * @return The index of the last character that fits in the region plus 1
		 */
		[[nodiscard]] auto FormatRange(std::optional<FORMATRANGE> formatRange, bool display = true) const noexcept -> std::int64_t;

		[[nodiscard]] auto GetDefaultCharFormat() const noexcept -> CHARFORMAT2W { return charFormat; }
		void SetDefaultCharFormat(const CHARFORMAT2W& charFormat) noexcept;

		[[nodiscard]] auto GetEventMask() const noexcept -> EditEventMaskFlag;
		void SetEventMask(EditEventMaskFlag eventFlags) const noexcept;

		[[nodiscard]] auto GetFirstVisibleLine() const noexcept -> std::int64_t;

		[[nodiscard]] auto GetTextLimit() const noexcept -> std::int64_t;
		void SetTextLimit(std::int64_t limit) const noexcept;

		[[nodiscard]] auto GetLine(int line) const noexcept -> std::wstring;

		[[nodiscard]] auto GetOptions()  const noexcept -> EditOptionsFlag;
		void SetOptions(EditOptionsFlag options) const noexcept;

		[[nodiscard]] auto GetParaFormat() const noexcept -> PARAFORMAT2 { return paraFormat; }
		void SetParaFormat(const PARAFORMAT2& paraFormat) noexcept;

		[[nodiscard]] auto GetFormattingRect() const noexcept -> RectL;
		void SetFormattingRect(RectL rect) const noexcept;

		[[nodiscard]] auto GetRedoName() const noexcept -> UNDONAMEID;
		[[nodiscard]] auto GetUndoName() const noexcept -> UNDONAMEID;

		/**
		 * @return first is beginning and the last is ending character
		 * If start is 0 and end is -1 all text is selected
		 */
		[[nodiscard]] auto GetSelection() const noexcept -> CharRange;
		void SetSelection(CharRange charRange) const noexcept;

		[[nodiscard]] auto GetSelectionCharFormat() const noexcept -> CHARFORMAT2W;
		void SetSelectionCharFormat(const CHARFORMAT2W& charFormat) const noexcept;

		void SetWordCharFormat(const CHARFORMAT2W& charFormat) const noexcept;

		[[nodiscard]] auto GetSelectionType() const noexcept -> EditSelectionFlag;

		[[nodiscard]] auto GetSelectedText() const noexcept -> std::wstring;

		[[nodiscard]] auto GetTextLength(
			std::optional<GETTEXTLENGTHEX> lengthEx = std::nullopt) const noexcept -> std::int64_t;

		[[nodiscard]] auto GetTextMode() const noexcept -> TEXTMODE;
		void SetTextMode(TEXTMODE textMode) const noexcept;

		[[nodiscard]] auto GetTextRange(CharRange charRange) const noexcept -> std::wstring;

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
		[[nodiscard]] auto GetLineFromCharIndex(std::int64_t index = -1) const noexcept -> std::int64_t;
		/**
		 * @param line - 0 based line index 
		 * If line is -1 it specifies the current line number (the line that contains the caret)
		 * @return Character index of the line 
		 * If it is -1 the specified line number is greater than the number of lines in the edit control
		 */
		[[nodiscard]] auto GetLineIndex(std::int64_t line = -1) const noexcept -> std::int64_t;
		/**
		 * @param line - 0 based index of the character in the line
		 * If it is -1 returns the number of unselected characters on lines containing selected characters
		 * Look at https://learn.microsoft.com/en-us/windows/win32/Controls/em-linelength for more info
		 * @return Number of characters in the line
		 */
		[[nodiscard]] auto GetLineLength(std::int64_t index = -1) const noexcept -> std::int64_t;

		void ScrollLines(std::int64_t lines) const noexcept;
		
		void RequestSize() const noexcept;

		[[nodiscard]] auto GetRichEditOle() const noexcept -> ComPtr<IRichEditOle>;
		void SetOleCallack(ComPtr<IRichEditOleCallback> callback) const noexcept;

		void EnableAutoURLDetect() const noexcept;
		void DisableAutoURLDetect() const noexcept;

		[[nodiscard]] auto StreamIn(int streamFormat, EDITSTREAM editStream) const noexcept -> std::int64_t;
		[[nodiscard]] auto StreamOut(int streamFormat, EDITSTREAM editStream) const noexcept -> std::int64_t;

		#pragma endregion

		#pragma region Events

		auto ChangedEvent() -> auto& { return changedEvent; }
		auto ClipFormatEvent() -> auto& { return clipFormatEvent; }
		auto DragDropDoneEvent() -> auto& { return dragDropDoneEvent; }
		auto DropFilesEvent() -> auto& { return dropFilesEvent; }
		auto LinkEvent() -> auto& { return linkEvent; }
		auto LowFirtfEvent() -> auto& { return lowFirtfEvent; }
		auto ObjectPositionsEvent() -> auto& { return objectPositionsEvent; }
		auto OleOpFailedEvent() -> auto& { return oleOpFailedEvent; }
		auto ParagraphExpandedEvent() -> auto& { return paragraphExpandedEvent; }
		auto ProtectedEvent() -> auto& { return protectedEvent; }
		auto RequestResizeEvent() -> auto& { return requestResizeEvent; }
		auto ScrollEvent() -> auto& { return scrollEvent; }
		auto SelectionChangeEvent() -> auto& { return selectionChangeEvent; }
		auto UpdateEvent() -> auto& { return updateEvent; }
		auto ErrSpaceEvent() -> auto& { return errSpaceEvent; }
		auto MaxTextEvent() -> auto& { return maxTextEvent; }
		auto CaretPositionChangedEvent() -> auto& { return caretPositionChangedEvent; }

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

		auto OnDPIChange(float dpiScale, RectI suggestedRect) -> Core::HandlerResult override;
		auto ForwardToTextServices(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnSetCursor(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnSetFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnKillFocus(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};

	namespace BuiltinFilters
	{
		auto NumericOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept -> bool;
		auto UppercaseOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept -> bool;
		auto LowercaseOnlyFilter(Core::WindowPtr<Edit> edit, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept -> bool;
	}
}
