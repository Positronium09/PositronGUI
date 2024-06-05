#include "ui/Control.hpp"


namespace PGUI::UI
{
	Control::Control(const Core::WindowClass::WindowClassPtr& wndClass) noexcept : 
		UIComponent{ wndClass }
	{
		RegisterMessageHandler(WM_CREATE, &Control::OnCreate);
	}
	Control::Control() noexcept
	{
		RegisterMessageHandler(WM_CREATE, &Control::OnCreate);
	}

	Core::HandlerResult Control::OnCreate(UINT, WPARAM, LPARAM) const noexcept
	{
		// Controls have input 
		// d by default
		EnableInput();
		return 0;
	}
}
