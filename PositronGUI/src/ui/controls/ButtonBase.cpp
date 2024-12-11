#include "ui/controls/ButtonBase.hpp"


namespace PGUI::UI::Controls
{
	Controls::ButtonBase::ButtonBase(const Core::WindowClass::WindowClassPtr& wndClass) noexcept : 
		PGUI::UI::Control{ wndClass }
	{
		RegisterMessageHandler(WM_MOUSEMOVE, &ButtonBase::OnMouseMove);
		RegisterMessageHandler(WM_MOUSELEAVE, &ButtonBase::OnMouseLeave);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ButtonBase::OnLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &ButtonBase::OnLButtonUp);
	}

	Controls::ButtonBase::ButtonBase() noexcept
	{
		RegisterMessageHandler(WM_MOUSEMOVE, &ButtonBase::OnMouseMove);
		RegisterMessageHandler(WM_MOUSELEAVE, &ButtonBase::OnMouseLeave);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ButtonBase::OnLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &ButtonBase::OnLButtonUp);
	}

	auto Controls::ButtonBase::ClickedEvent() noexcept -> Core::Event<void>&
	{
		return clickedEvent;
	}
	auto Controls::ButtonBase::StateChangedEvent() noexcept -> Core::Event<ButtonState>&
	{
		return stateChangedEvent;
	}

	void Controls::ButtonBase::SetState(ButtonState _state) noexcept
	{
		state = _state;
		stateChangedEvent.Emit(state);
	}
	void ButtonBase::SetMouseState(ButtonState _state) noexcept
	{
		state = _state | GetSelectionState();
		stateChangedEvent.Emit(state);
	}
	void ButtonBase::SetSelectionState(ButtonState _state) noexcept
	{
		state = _state | GetMouseState();
		stateChangedEvent.Emit(state);
	}

	auto Controls::ButtonBase::GetState() const noexcept -> ButtonState
	{
		return state;
	}
	auto ButtonBase::GetMouseState() const noexcept -> ButtonState
	{
		return (state & ButtonState::Normal) | (state & ButtonState::Hover) | (state & ButtonState::Pressed);
	}
	auto ButtonBase::GetSelectionState() const noexcept -> ButtonState
	{
		return (state & ButtonState::Unchecked) | (state & ButtonState::Checked) | (state & ButtonState::Indeterminate);
	}
	
	auto ButtonBase::OnMouseMove(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		if (IsFlagSet(state, ButtonState::Pressed))
		{
			return 0;
		}

		SetMouseState(ButtonState::Hover);

		TRACKMOUSEEVENT tme{ };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.hwndTrack = Hwnd();
		tme.dwHoverTime = 0;
		tme.dwFlags = TME_LEAVE;

		TrackMouseEvent(&tme);
		
		return 0;
	}

	auto Controls::ButtonBase::OnMouseLeave(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		SetMouseState(ButtonState::Normal);

		return 0;
	}

	auto Controls::ButtonBase::OnLButtonDown(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		SetMouseState(ButtonState::Pressed);

		return 0;
	}

	auto ButtonBase::OnLButtonUp(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		if (IsFlagSet(state, ButtonState::Pressed))
		{
			clickedEvent.Emit();
		}

		SetMouseState(ButtonState::Hover);

		return 0;
	}
}
