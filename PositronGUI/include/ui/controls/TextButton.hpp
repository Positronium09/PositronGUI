#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"
#include "ui/Brush.hpp"
#include "ui/UIColors.hpp"
#include "ui/controls/ButtonBase.hpp"


namespace PGUI::UI::Controls
{
	class TextButton : public ButtonBase
	{
		public:
		struct TextButtonColors
		{
			BrushParameters normalText;
			BrushParameters normalBackground;
			BrushParameters hoverText;
			BrushParameters hoverBackground;
			BrushParameters clickedText;
			BrushParameters clickedBackground;

			TextButtonColors() = default;
		};

		[[nodiscard]] static TextButton::TextButtonColors GetTextButtonColors() noexcept;
		[[nodiscard]] static TextButton::TextButtonColors GetTextButtonAccentedColors() noexcept;

		explicit TextButton(const TextButtonColors& colors = GetTextButtonColors(), 
			TextFormat textFormat = TextFormat{ }) noexcept;

		[[nodiscard]] TextLayout GetTextLayout() const noexcept;
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view text) noexcept;
		[[nodiscard]] const std::wstring& GetText() const noexcept;

		[[nodiscard]] const TextButtonColors& GetColors() const noexcept;
		[[nodiscard]] TextButtonColors& GetColors() noexcept;

		[[nodiscard]] Core::Event<std::wstring_view>& TextChangedEvent() noexcept;

		protected:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		private:
		TextButtonColors colors;

		std::wstring text;
		TextFormat textFormat;
		TextLayout textLayout;

		Brush textBrush;
		Brush backgroundBrush;

		Core::Event<std::wstring_view> textChangedEvent;

		void OnStateChanged(ButtonState state) noexcept;

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

		Core::HandlerResult OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
}
