#include "ui/Control.hpp"


namespace PGUI::UI
{
	Control::Control() noexcept
	{
		RegisterMessageHandler(WM_CREATE, &Control::OnCreate);
	}

	Core::HandlerResult Control::OnCreate(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam) const
	{
		// Controls have input enabled by default
		EnableInput();
		return 0;
	}
}
