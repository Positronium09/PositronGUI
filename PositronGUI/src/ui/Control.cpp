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

	auto Control::OnCreate(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const noexcept -> Core::HandlerResult
	{
		// Controls have input enabled by default
		EnableInput();
		return 0;
	}
	auto Control::OnLButtonDown(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const noexcept -> Core::HandlerResult
	{
		SetFocus(Hwnd());

		return 0;
	}
}
