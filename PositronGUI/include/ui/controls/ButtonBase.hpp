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
		Normal = 0x00,
		Unchecked = 0x00,
		Hover = 0x01,
		Pressed = 0x02,
		Selected = 0x04,
		Checked = 0x04,
		Indeterminate = 0x08
	};
	EnableEnumFlag(ButtonState);

	class ButtonBase : public Control
	{
		public:
		explicit ButtonBase(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;
		ButtonBase() noexcept;

		[[nodiscard]] Core::Event<void>& ClickedEvent() noexcept;
		[[nodiscard]] Core::Event<ButtonState>& StateChangedEvent() noexcept;

		[[nodiscard]] ButtonState GetState() const noexcept;
		[[nodiscard]] ButtonState GetMouseState() const noexcept;
		[[nodiscard]] ButtonState GetSelectionState() const noexcept;
		[[nodiscard]] ButtonState GetCheckedState() const noexcept { return GetSelectionState(); }

		void SetState(ButtonState state) noexcept;
		void SetMouseState(ButtonState state) noexcept;
		void SetSelectionState(ButtonState state) noexcept;
		void SetCheckedState(ButtonState state_) noexcept { SetSelectionState(state_); }

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
