#pragma once

#include "ui/UIComponent.hpp"


namespace PGUI::UI::Controls
{
	class HorizontalLayout : public UIComponent
	{
		public:
		enum class LayoutSetting
		{
			FillSpace,
			SpaceAround,
			SpaceBetween
		};

		explicit HorizontalLayout(LayoutSetting setting = LayoutSetting::FillSpace,
			long layoutGap = 30) noexcept;

		[[nodiscard]] auto GetLayoutSetting() const noexcept { return setting; }
		void SetLayoutSetting(LayoutSetting setting);

		[[nodiscard]] auto GetLayoutGap() const noexcept { return layoutGap; }
		void SetLayoutGap(long layoutGap);

		private:
		LayoutSetting setting;
		long layoutGap;

		void OnChildAdded(Core::WindowPtr<Core::Window> wnd) override;

		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;

		void RearrangeChildren() const;
	};
}
