#pragma once

#include "ui/bmp/Bitmap.hpp"
#include "ui/Dialog.hpp"
#include "ui/controls/TextButton.hpp"
#include "ui/controls/StaticText.hpp"
#include "ui/Colors.hpp"


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
		friend MessageBoxChoice ShowMessageBox(HWND parentHwnd,
			std::wstring_view caption, std::wstring_view text,
			MessageBoxButtonSet buttonSet, MessageBoxIcon icon);

		public:
		//! Dont use directly (you can but i dont recommend it)
		//! I can make this private but Dialog::Create wont be able to create the message dialog
		MessageBoxDialog(std::wstring_view text,
			MessageBoxButtonSet buttonSet, 
			MessageBoxIcon icon) noexcept;

		private:
		static inline const RectI margin{ 20, 20, 20, 20 };
		static inline const SizeI maxSize{ 600, 600 };
		static inline const SizeI buttonSize = { 85, 35 };

		std::wstring text;
		std::vector<Core::WindowPtr<Controls::TextButton>> buttons;

		Core::WindowPtr<Controls::StaticText> staticText;

		MessageBoxButtonSet buttonSet;
		MessageBoxChoice choice = MessageBoxChoice::Ok;

		Bmp::Bitmap icon;
		ComPtr<ID2D1Bitmap> iconBmp;

		Brush buttonHighlightBrush{ Colors::Black };
		RGBA backgroundColor;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;
		
		[[nodiscard]] MessageBoxChoice Display() noexcept;
		void CreateButtons();
		void SetToRequiredSize() const noexcept;
		void ButtonHandler(MessageBoxChoice choice) noexcept;

		SizeI CalculateSize() const noexcept;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
	};

	MessageBoxChoice ShowMessageBox(HWND parentHwnd,
		std::wstring_view caption, std::wstring_view text, 
		MessageBoxButtonSet buttonSet = MessageBoxButtonSet::Ok,
		MessageBoxIcon icon = MessageBoxIcon::None);
}
