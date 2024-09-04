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
}
EnableEnumFlag(PGUI::UI::Controls::ButtonState)

namespace PGUI::UI::Controls
{
	class ButtonBase : public Control
	{
		public:
		explicit ButtonBase(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;
		ButtonBase() noexcept;

		[[nodiscard]] auto ClickedEvent() noexcept -> Core::Event<void>&;
		[[nodiscard]] auto StateChangedEvent() noexcept -> Core::Event<ButtonState>&;

		[[nodiscard]] auto GetState() const noexcept -> ButtonState;
		[[nodiscard]] auto GetMouseState() const noexcept -> ButtonState;
		[[nodiscard]] auto GetSelectionState() const noexcept -> ButtonState;
		[[nodiscard]] auto GetCheckedState() const noexcept -> ButtonState { return GetSelectionState(); }

		void SetState(ButtonState state) noexcept;
		void SetMouseState(ButtonState state) noexcept;
		void SetSelectionState(ButtonState state) noexcept;
		void SetCheckedState(ButtonState state_) noexcept { SetSelectionState(state_); }

		private:
		ButtonState state = ButtonState::Normal;

		Core::Event<void> clickedEvent;
		Core::Event<ButtonState> stateChangedEvent;

		auto OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
	};
}
