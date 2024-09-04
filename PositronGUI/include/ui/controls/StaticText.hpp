#pragma once

#include "core/Event.hpp"
#include "ui/UIComponent.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"
#include "ui/Brush.hpp"

#include <string>


namespace PGUI::UI::Controls
{
	class StaticText : public UIComponent
	{
		public:
		explicit StaticText(TextFormat textFormat=TextFormat{ }) noexcept;

		[[nodiscard]] auto GetTextLayout() const noexcept -> TextLayout;
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view text) noexcept;
		[[nodiscard]] auto GetText() const noexcept -> const std::wstring&;
		[[nodiscard]] auto GetText() noexcept -> std::wstring&;

		void SetTextBrush(const Brush& textBrush) noexcept;
		[[nodiscard]] auto GetTextBrush() const noexcept -> const Brush&;

		void SetBackgroundBrush(const Brush& backgroundBrush) noexcept;
		[[nodiscard]] auto GetBackgroundBrush() const noexcept -> const Brush&;

		[[nodiscard]] auto TextChangedEvent() noexcept -> Core::Event<std::wstring_view>&;

		protected:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		private:
		std::wstring text;
		TextFormat textFormat;
		TextLayout textLayout;

		Brush textBrush;
		Brush backgroundBrush;

		Core::Event<std::wstring_view> textChangedEvent;

		auto OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult override;
		auto OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		
		auto OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
