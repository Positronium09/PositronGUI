#pragma once

#include "ui/bmp/Bitmap.hpp"
#include "ui/Dialog.hpp"
#include "ui/controls/TextButton.hpp"
#include "ui/controls/StaticText.hpp"
#include "ui/Colors.hpp"
#include "graphics/GraphicsBitmap.hpp"


namespace PGUI::UI::Dialogs
{
	enum class MessageBoxButtonSet
	{
		Ok,
		OkCancel,
		RetryCancel,
		YesNo,
		YesNoCancel,
		CancelTryContinue,
		AbortRetryIgnore,
		Help
	};
	enum class MessageBoxIcon
	{
		None = 0,
		Hand = 1,
		Stop = 1,
		Error = 1,
		Question = 2,
		Exclamation = 3,
		Warning = 3,
		Asterisk = 4,
		Information = 4
	};
	enum class MessageBoxChoice
	{
		Ok,
		Cancel,
		Continue,
		Ignore,
		Abort,
		Yes,
		No,
		Retry,
		TryAgain,
	};

	class MessageBoxDialog final : public ModalDialog
	{
		friend auto ShowMessageBox(HWND parentHwnd,
			std::wstring_view caption, std::wstring_view text,
			MessageBoxButtonSet buttonSet, MessageBoxIcon icon) -> MessageBoxChoice;

		public:
		//! Dont use directly (you can but i dont recommend it)
		//! I can make this private but Dialog::Create wont be able to create the message dialog
		MessageBoxDialog(std::wstring_view text,
			MessageBoxButtonSet buttonSet, 
			MessageBoxIcon icon) noexcept;

		private:
		static inline RectI margin{ 20, 20, 20, 20 };
		static inline SizeI maxSize{ 600, 600 };
		static inline SizeI buttonSize = { 85, 35 };
		static inline SizeI iconSize = { 32, 32 };

		std::wstring text;
		std::vector<Core::WindowPtr<Controls::TextButton>> buttons;

		TextLayout textLayout;
		TextFormat textFormat;

		MessageBoxButtonSet buttonSet;
		MessageBoxIcon _icon;
		MessageBoxChoice choice = MessageBoxChoice::Ok;

		Bmp::Bitmap icon;
		Graphics::GraphicsBitmap iconBmp;

		Brush buttonHighlightBrush;
		Brush textBrush;
		RGBA backgroundColor;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;
		
		[[nodiscard]] auto Display() noexcept -> MessageBoxChoice;
		void CreateButtons();
		void SetToRequiredSize() const noexcept;
		void ButtonHandler(MessageBoxChoice choice) noexcept;

		[[nodiscard]] auto CalculateSize() const noexcept -> SizeI;

		auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
	};

	auto ShowMessageBox(HWND parentHwnd,
		std::wstring_view caption, std::wstring_view text, 
		MessageBoxButtonSet buttonSet = MessageBoxButtonSet::Ok,
		MessageBoxIcon icon = MessageBoxIcon::None) -> MessageBoxChoice;
}
