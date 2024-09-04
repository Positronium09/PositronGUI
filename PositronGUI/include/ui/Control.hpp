#pragma once

#include "UIComponent.hpp"


namespace PGUI::UI
{
	class Control : public UIComponent
	{
		public:
		explicit Control(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;
		Control() noexcept;

		private:
		[[nodiscard]] auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
