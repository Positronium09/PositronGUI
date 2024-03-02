#pragma once

#include "Style.hpp"
#include "ClipGeometry.hpp"
#include "core/DirectCompositionWindow.hpp"


namespace PGUI::UI
{
	class UIComponent : public Core::DirectCompositionWindow
	{
		public:
		explicit UIComponent(const Core::WindowClass::WindowClassPtr& wndClass);
		UIComponent();

		void SetClipGeometry(Clip geometry);
		void ClearClipGeometry();
		void HitTestClipGeometry(bool enable);

		[[nodiscard]] Style& GetStyle();
		[[nodiscard]] const Style& GetStyle() const;
		void SetStyle(const Style& style);

		protected:
		void BeginDraw() override;
		HRESULT EndDraw() override;

		private:
		Style style;

		Clip clip;
		bool hitTestClipGeometry = true;

		Core::HandlerResult OnNcHitTest(UINT msg, WPARAM wParam, LPARAM lParam) const;
	};
}
