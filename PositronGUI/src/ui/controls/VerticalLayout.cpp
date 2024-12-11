#include "ui/controls/VerticalLayout.hpp"


namespace PGUI::UI::Controls
{
	VerticalLayout::VerticalLayout(LayoutSetting setting, long layoutGap) noexcept :
		UIComponent{ Core::WindowClass::Create(L"VerticalLayout_UIComponent") },
		setting{ setting }, layoutGap{ layoutGap }
	{
		RegisterMessageHandler(WM_SIZE, &VerticalLayout::OnSize);
	}

	void VerticalLayout::SetLayoutSetting(LayoutSetting _setting)
	{
		setting = _setting;
		RearrangeChildren();
	}

	void VerticalLayout::SetLayoutGap(long _layoutGap)
	{
		layoutGap = _layoutGap;
		RearrangeChildren();
	}

	void VerticalLayout::OnChildAdded(Core::WindowPtr<Core::Window> /* unused */)
	{
		RearrangeChildren();
	}

	auto VerticalLayout::OnSize(UINT /* unused */, WPARAM /* unused */, LPARAM /* unused */) noexcept -> Core::HandlerResult
	{
		RearrangeChildren();

		return 0;
	}

	void VerticalLayout::RearrangeChildren() const
	{
		auto& children = GetChildWindowList();
		auto childCount = children.size();

		if (childCount == 0)
		{
			return;
		}

		auto size = GetClientSize();

		switch (setting)
		{
			using enum LayoutSetting;
			case FillSpace:
			{
				size.cy = static_cast<long>(size.cy / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ 0, static_cast<long>(index * size.cy) });
				}
				break;
			}

			case SpaceBetween:
			{
				size.cy = static_cast<long>((size.cy - layoutGap * (childCount - 1)) / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ 0, static_cast<long>(index * (size.cy + layoutGap)) });
				}
				break;
			}

			case SpaceAround:
			{
				size.cy = static_cast<long>((size.cy - layoutGap * (childCount + 1)) / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ 0, static_cast<long>(index * (size.cy + layoutGap)) + layoutGap });
				}
				break;
			}

			default:
				std::unreachable();
		}
	}
}
