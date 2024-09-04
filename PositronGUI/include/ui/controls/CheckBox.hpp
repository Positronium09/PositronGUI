#pragma once

#include "ui/controls/ButtonBase.hpp"


namespace PGUI::UI::Controls
{
	class CheckBox : public ButtonBase
	{
		public:
		//? Foreground colors used as borders in unchecked state
		struct CheckBoxStateColors
		{
			BrushParameters foreground;
			BrushParameters background;
			BrushParameters hoverForeground;
			BrushParameters hoverBackground;
			BrushParameters pressedForeground;
			BrushParameters pressedBackground;
		};
		struct CheckBoxColors
		{
			CheckBoxStateColors checked;
			CheckBoxStateColors unchecked;
			CheckBoxStateColors indeterminate;

			CheckBoxColors() = default;
		};

		[[nodiscard]] static auto GetCheckBoxColors() noexcept -> CheckBox::CheckBoxColors;
		[[nodiscard]] static auto GetCheckBoxAccentedColors() noexcept -> CheckBox::CheckBoxColors;

		explicit CheckBox(CheckBoxColors  colors = GetCheckBoxAccentedColors()) noexcept;

		[[nodiscard]] auto GetColors() const noexcept -> const CheckBoxColors& { return colors; }
		[[nodiscard]] auto GetColors() noexcept -> CheckBoxColors& { return colors; }
		void SetColors(const CheckBoxColors& newColors) { colors = newColors; }

		[[nodiscard]] auto IsTriState() const noexcept -> bool { return isTriState; }
		void SetTriState(bool triState = true) noexcept { isTriState = triState; }

		private:
		bool isTriState = false;
		CheckBoxColors colors;

		Brush backgroundBrush;
		Brush foregroundBrush;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		void OnClicked() noexcept;
		void OnStateChanged(ButtonState state) noexcept;

		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
	};
}
