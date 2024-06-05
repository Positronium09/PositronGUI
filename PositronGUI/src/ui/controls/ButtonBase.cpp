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

	Core::Event<void>& Controls::ButtonBase::ClickedEvent() noexcept
	{
		return clickedEvent;
	}
	Core::Event<ButtonState>& Controls::ButtonBase::StateChangedEvent() noexcept
	{
		return stateChangedEvent;
	}

	void Controls::ButtonBase::SetState(ButtonState _state) noexcept
	{
		state = _state;
		stateChangedEvent.Emit(state);
	}
	ButtonState Controls::ButtonBase::GetState() const noexcept
	{
		return state;
	}
	
	Core::HandlerResult ButtonBase::OnMouseMove(UINT, WPARAM, LPARAM) noexcept
	{
		if (state == ButtonState::Clicked)
		{
			return 0;
		}

		SetState(ButtonState::Hover);

		TRACKMOUSEEVENT tme{ };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.hwndTrack = Hwnd();
		tme.dwHoverTime = 0;
		tme.dwFlags = TME_LEAVE;

		TrackMouseEvent(&tme);
		
		return 0;
	}

	Core::HandlerResult Controls::ButtonBase::OnMouseLeave(UINT, WPARAM, LPARAM) noexcept
	{
		SetState(ButtonState::Normal);

		return 0;
	}

	Core::HandlerResult Controls::ButtonBase::OnLButtonDown(UINT, WPARAM, LPARAM) noexcept
	{
		SetState(ButtonState::Clicked);

		return 0;
	}

	Core::HandlerResult ButtonBase::OnLButtonUp(UINT, WPARAM, LPARAM) noexcept
	{
		if (state == ButtonState::Clicked)
		{
			clickedEvent.Emit();
		}

		SetState(ButtonState::Hover);

		return 0;
	}
}
