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

		[[nodiscard]] static auto GetTextButtonColors() noexcept -> TextButton::TextButtonColors;
		[[nodiscard]] static auto GetTextButtonAccentedColors() noexcept -> TextButton::TextButtonColors;

		explicit TextButton(TextButtonColors  colors = GetTextButtonColors(), 
			TextFormat textFormat = TextFormat{ }) noexcept;

		[[nodiscard]] auto GetTextLayout() const noexcept -> TextLayout;
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view text) noexcept;
		[[nodiscard]] auto GetText() const noexcept -> const std::wstring&;

		[[nodiscard]] auto GetColors() const noexcept -> const TextButtonColors&;
		[[nodiscard]] auto GetColors() noexcept -> TextButtonColors&;

		[[nodiscard]] auto TextChangedEvent() noexcept -> Core::Event<std::wstring_view>&;

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

		auto OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult override;
		auto OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;

		auto OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
