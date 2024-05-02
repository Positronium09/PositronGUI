#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"
#include "ui/Brush.hpp"
#include "ui/UIColors.hpp"


namespace PGUI::UI::Controls
{
	enum class ButtonState
	{
		Normal = 0,
		Hover = 1,
		Clicked = 2,
	};

	class ButtonBase : public UI::Control
	{
		public:
		explicit ButtonBase(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;
		ButtonBase() noexcept;

		[[nodiscard]] Core::Event<void>& ClickedEvent() noexcept;
		[[nodiscard]] Core::Event<ButtonState>& StateChangedEvent() noexcept;

		protected:
		void SetState(ButtonState state) noexcept;
		[[nodiscard]] ButtonState GetState() const noexcept;

		private:
		ButtonState state = ButtonState::Normal;

		Core::Event<void> clickedEvent;
		Core::Event<ButtonState> stateChangedEvent;

		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
