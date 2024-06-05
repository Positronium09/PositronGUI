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

		[[nodiscard]] TextLayout GetTextLayout() const noexcept;
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view text) noexcept;
		[[nodiscard]] const std::wstring& GetText() const noexcept;
		[[nodiscard]] std::wstring& GetText() noexcept;

		void SetTextBrush(const Brush& textBrush) noexcept;
		[[nodiscard]] const Brush& GetTextBrush() const noexcept;

		void SetBackgroundBrush(const Brush& backgroundBrush) noexcept;
		[[nodiscard]] const Brush& GetBackgroundBrush() const noexcept;

		Core::Event<std::wstring_view>& TextChangedEvent() noexcept;

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


		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		
		Core::HandlerResult OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
}
