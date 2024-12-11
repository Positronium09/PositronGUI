#include "ui/controls/HorizontalLayout.hpp"


namespace PGUI::UI::Controls
{
	HorizontalLayout::HorizontalLayout(LayoutSetting setting, long layoutGap) noexcept :
		UIComponent{ Core::WindowClass::Create(L"HorizontalLayout_UIComponent") },
		setting{ setting }, layoutGap{ layoutGap }
	{
		RegisterMessageHandler(WM_SIZE, &HorizontalLayout::OnSize);
	}

	void HorizontalLayout::SetLayoutSetting(LayoutSetting _setting)
	{
		setting = _setting;
		RearrangeChildren();
	}

	void HorizontalLayout::SetLayoutGap(long _layoutGap)
	{
		layoutGap = _layoutGap;
		RearrangeChildren();
	}

	void HorizontalLayout::OnChildAdded(Core::WindowPtr<Core::Window> /* unused */)
	{
		RearrangeChildren();
	}

	auto HorizontalLayout::OnSize(UINT /* unused */, WPARAM /* unused */, LPARAM /* unused */) noexcept -> Core::HandlerResult
	{
		RearrangeChildren();

		return 0;
	}

	void HorizontalLayout::RearrangeChildren() const
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
				size.cx = static_cast<long>(size.cx / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ static_cast<long>(index * size.cx), 0 });
				}
				break;
			}

			case SpaceBetween:
			{
				size.cx = static_cast<long>((size.cx - layoutGap * (childCount - 1)) / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ static_cast<long>(index * (size.cx + layoutGap)), 0 });
				}
				break;
			}

			case SpaceAround:
			{
				size.cx = static_cast<long>((size.cx - layoutGap * (childCount + 1)) / childCount);

				for (const auto& [index, child] : children | std::views::enumerate)
				{
					child->Resize(size);
					child->Move({ static_cast<long>(index * (size.cx + layoutGap)) + layoutGap, 0 });
				}
				break;
			}

			default:
				std::unreachable();
		}
	}
}
