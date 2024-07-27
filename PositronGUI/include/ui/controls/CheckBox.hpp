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

		[[nodiscard]] static CheckBox::CheckBoxColors GetCheckBoxColors() noexcept;
		[[nodiscard]] static CheckBox::CheckBoxColors GetCheckBoxAccentedColors() noexcept;

		explicit CheckBox(const CheckBoxColors& colors = GetCheckBoxColors()) noexcept;

		[[nodiscard]] const CheckBoxColors& GetColors() const noexcept { return colors; }
		[[nodiscard]] CheckBoxColors& GetColors() noexcept { return colors; }
		void SetColors(const CheckBoxColors& newColors) { colors = newColors; }

		[[nodiscard]] bool IsTriState() const noexcept { return isTriState; }
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

		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
