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

		colors.checked.foreground = RGBA{ 0x333333 };
		colors.checked.background = RGBA{ 0x151515 };
		colors.checked.hoverForeground = colors.checked.foreground;
		colors.checked.hoverBackground = RGBA{ 0x1b1b1b };
		colors.checked.pressedForeground = RGBA{ 0x272727 };
		colors.checked.pressedBackground = colors.checked.hoverBackground;

		colors.indeterminate = colors.checked;

		return colors;
	}
	CheckBox::CheckBoxColors CheckBox::GetCheckBoxAccentedColors() noexcept
	{
		CheckBoxColors colors;

		colors.unchecked.foreground = RGBA{ 0x333333 };
		colors.unchecked.background = RGBA{ 0x1b1b1b };
		colors.unchecked.hoverForeground = colors.unchecked.foreground;
		colors.unchecked.hoverBackground = RGBA{ 0x202020 };
		colors.unchecked.pressedForeground = RGBA{ 0x292929 };
		colors.unchecked.pressedBackground = colors.unchecked.hoverBackground;

		colors.checked.foreground = RGBA{ 0x1b1b1b };
		colors.checked.background = UIColors::GetAccentColor();
		colors.checked.hoverForeground = colors.checked.foreground;
		colors.checked.hoverBackground = UIColors::GetAccentLight1Color();
		colors.checked.pressedForeground = colors.checked.hoverForeground;
		colors.checked.pressedBackground = UIColors::GetAccentDark1Color();

		colors.indeterminate = colors.checked;

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
		auto g = GetGraphics();

		if (!foregroundBrush)
		{
			SetGradientBrushRect(foregroundBrush, GetClientRect());
			g.CreateBrush(foregroundBrush);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
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

		auto g = GetGraphics();

		auto clientRect = RoundedRect{ GetClientRect() };
		auto clientSize = clientRect.Size();
		
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
				g.FillRoundedRect(clientRect, backgroundBrush);
				g.DrawRoundedRect(clientRect, foregroundBrush, ScaleByDpi(2.5f));
				break;
			}
			case ButtonState::Checked:
			{
				g.FillRoundedRect(clientRect, backgroundBrush);

				auto prevTransform = g.GetTransform();

				auto center = clientRect.Center();
				auto scaleTransform = GetDpiScaleTransform() * D2D1::Matrix3x2F::Scale(clientSize / 50, center);
				RectF rc{ center.x - ScaleByDpi(10.f),
					center.y, center.x,
					center.y + ScaleByDpi(3.0f) };
				rc.Shift(ScaleByDpi(4.0f), ScaleByDpi(3.0f));
				g.SetTransform(D2D1::Matrix3x2F::Rotation(45, center) * scaleTransform);
				g.FillRect(rc, foregroundBrush);

				rc = RectF{ center.x - ScaleByDpi(3.0f),
					center.y - ScaleByDpi(17.0f),
					center.x, center.y };
				rc.Shift(ScaleByDpi(4.0f), ScaleByDpi(5.0f));
				g.SetTransform(D2D1::Matrix3x2F::Rotation(42, center) * scaleTransform);
				g.FillRect(rc, foregroundBrush);

				g.SetTransform(prevTransform);

				break;
			}
			case ButtonState::Indeterminate:
			{
				g.Clear(backgroundBrush);

				RoundedRect rect{ clientRect.Center(), SizeL{ 1, 1 } };
				
				rect.Inflate(12 * (clientSize.cx / 50), 3 * (clientSize.cy / 50));
				if (clientRect.xRadius != 0)
				{
					rect.xRadius = 2.5f * (clientSize.cx / 50);
				}
				if (clientRect.yRadius != 0)
				{
					rect.yRadius = 5.0f * (clientSize.cy / 50);
				}
				auto prevTransform = g.GetTransform();
				g.SetTransform(GetDpiScaleTransform());

				g.FillRoundedRect(rect, foregroundBrush);

				g.SetTransform(prevTransform);

				break;
			}
			default:
				break;
		}

		EndDraw();

		return 0;
	}
}
