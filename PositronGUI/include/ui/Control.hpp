#pragma once

#include "UIComponent.hpp"


namespace PGUI::UI
{
	class Control : public UIComponent
	{
		public:
		Control() noexcept;

		private:
		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) const;
	};
}
