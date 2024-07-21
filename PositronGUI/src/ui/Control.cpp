#include "ui/Control.hpp"


namespace PGUI::UI
{
	Control::Control(const Core::WindowClass::WindowClassPtr& wndClass) noexcept : 
		UIComponent{ wndClass }
	{
		RegisterMessageHandler(WM_CREATE, &Control::OnCreate);
		RegisterMessageHandler(WM_LBUTTONDOWN, &Control::OnLButtonDown);
	}
	Control::Control() noexcept
	{
		RegisterMessageHandler(WM_CREATE, &Control::OnCreate);
		RegisterMessageHandler(WM_LBUTTONDOWN, &Control::OnLButtonDown);
	}

	Core::HandlerResult Control::OnCreate(UINT, WPARAM, LPARAM) const noexcept
	{
		// Controls have input enabled by default
		EnableInput();
		return 0;
	}
	Core::HandlerResult Control::OnLButtonDown(UINT, WPARAM, LPARAM) const noexcept
	{
		SetFocus(Hwnd());

		return 0;
	}
}
