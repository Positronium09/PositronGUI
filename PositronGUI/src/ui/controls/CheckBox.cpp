#include "ui/controls/CheckBox.hpp"

#include "ui/Colors.hpp"
#include "ui/UIColors.hpp"

namespace PGUI::UI::Controls
{
	CheckBox::CheckBoxColors CheckBox::GetCheckBoxColors() noexcept
	{
		CheckBoxColors colors;

		colors.unchecked.foreground = RGBA{ 0x333333 };
		colors.unchecked.background = RGBA{ 0x1b1b1b };
		colors.unchecked.hoverForeground = colors.unchecked.foreground;
		colors.unchecked.hoverBackground = RGBA{ 0x202020 };
		colors.unchecked.pressedForeground = RGBA{ 0x292929 };
		colors.unchecked.pressedBackground = colors.unchecked.hoverBackground;

		colors.indeterminate.foreground = RGBA{ 0x333333 };
		colors.indeterminate.background = RGBA{ 0x151515 };
		colors.indeterminate.hoverForeground = colors.indeterminate.foreground;
		colors.indeterminate.hoverBackground = RGBA{ 0x1b1b1b };
		colors.indeterminate.pressedForeground = RGBA{ 0x272727 };
		colors.indeterminate.pressedBackground = colors.indeterminate.hoverBackground;

		colors.checked = colors.indeterminate;

		return colors;
	}
	CheckBox::CheckBoxColors CheckBox::GetCheckBoxAccentedColors() noexcept
	{
		CheckBoxColors colors;



		return colors;
	}

	CheckBox::CheckBox(const CheckBoxColors& colors) noexcept :
		colors{ colors }
	{
		RegisterMessageHandler(WM_PAINT, &CheckBox::OnPaint);

		ClickedEvent().Subscribe(PGUI::BindMemberFunc(&CheckBox::OnClicked, this));
		StateChangedEvent().Subscribe(PGUI::BindMemberFunc(&CheckBox::OnStateChanged, this));
		OnStateChanged(GetState());
	}

	void CheckBox::CreateDeviceResources()
	{
		auto renderer = GetRenderingInterface();

		if (!foregroundBrush)
		{
			SetGradientBrushRect(foregroundBrush, GetClientRect());
			foregroundBrush.CreateBrush(renderer);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			backgroundBrush.CreateBrush(renderer);
		}
	}

	void CheckBox::DiscardDeviceResources()
	{
		foregroundBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	void CheckBox::OnClicked() noexcept
	{
		switch (GetSelectionState())
		{
			case ButtonState::Unchecked:
			{
				SetSelectionState(ButtonState::Checked);
				break;
			}
			case ButtonState::Checked:
			{
				SetSelectionState(isTriState ? ButtonState::Indeterminate : ButtonState::Unchecked);
				break;
			}
			case ButtonState::Indeterminate:
			{
				SetSelectionState(ButtonState::Unchecked);
				break;
			}
			default:
				break;
		}
	}

	void CheckBox::OnStateChanged(ButtonState) noexcept
	{
		CheckBoxStateColors stateColors{ };
		switch (GetSelectionState())
		{
			case ButtonState::Unchecked:
			{
				stateColors = colors.unchecked;
				break;
			}
			case ButtonState::Checked:
			{
				stateColors = colors.checked;
				break;
			}
			case ButtonState::Indeterminate:
			{
				stateColors = colors.indeterminate;
				break;
			}
			default:
				return;
		}
		switch (GetMouseState())
		{
			case ButtonState::Normal:
			{
				foregroundBrush.SetParameters(stateColors.foreground);
				backgroundBrush.SetParameters(stateColors.background);
				break;
			}
			case ButtonState::Hover:
			{
				foregroundBrush.SetParameters(stateColors.hoverForeground);
				backgroundBrush.SetParameters(stateColors.hoverBackground);
				break;
			}
			case ButtonState::Pressed:
			{
				foregroundBrush.SetParameters(stateColors.pressedForeground);
				backgroundBrush.SetParameters(stateColors.pressedBackground);
				break;
			}
			default:
				return;
		}

		DiscardDeviceResources();
		Invalidate();
	}

	Core::HandlerResult CheckBox::OnPaint(UINT, WPARAM, LPARAM) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		auto clientRect = RoundedRect{ GetClientRect() };
		
		if (auto clipParams = GetClip().GetParameters();
			std::holds_alternative<RoundedRectangeClipParameters>(clipParams))
		{
			const auto& param = std::get<RoundedRectangeClipParameters>(clipParams);
			clientRect.xRadius = param.roundedRect.xRadius;
			clientRect.yRadius = param.roundedRect.yRadius;
		}

		switch (GetSelectionState())
		{
			case ButtonState::Unchecked:
			{
				renderer->FillRoundedRectangle(clientRect, backgroundBrush->GetBrushPtr());
				renderer->DrawRoundedRectangle(clientRect, foregroundBrush->GetBrushPtr(), 2.5f);
				break;
			}
			case ButtonState::Checked:
			{
				renderer->FillRoundedRectangle(clientRect, backgroundBrush->GetBrushPtr());

				D2D1::Matrix3x2F prevTransform{ };
				renderer->GetTransform(&prevTransform);

				auto center = clientRect.Center();
				RectF rc{ center.x - 10, center.y, center.x, center.y + 3 };
				rc.Shift(4, 3);
				renderer->SetTransform(D2D1::Matrix3x2F::Rotation(45, center));
				renderer->FillRectangle(rc, foregroundBrush->GetBrushPtr());

				rc = RectF{ center.x - 3, center.y - 17, center.x, center.y };
				rc.Shift(4, 5);
				renderer->SetTransform(D2D1::Matrix3x2F::Rotation(42, center));
				renderer->FillRectangle(rc, foregroundBrush->GetBrushPtr());

				renderer->SetTransform(prevTransform);

				break;
			}
			case ButtonState::Indeterminate:
			{
				renderer->FillRoundedRectangle(clientRect, backgroundBrush->GetBrushPtr());

				RoundedRect rect{ clientRect.Center(), SizeL{ 1, 1 } };
				rect.Inflate(12, 3);
				if (clientRect.xRadius != 0)
				{
					rect.xRadius = 2.5f;
				}
				if (clientRect.yRadius != 0)
				{
					rect.yRadius = 5.0f;
				}
				renderer->FillRoundedRectangle(rect, foregroundBrush->GetBrushPtr());
				break;
			}
			default:
				break;
		}

		EndDraw();

		return 0;
	}
}
