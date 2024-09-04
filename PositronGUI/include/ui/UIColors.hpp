#pragma once

#include "helpers/ComPtr.hpp"
#include "ui/Color.hpp"

#include <memory>
#include <winrt/windows.ui.viewmanagement.h>



namespace PGUI::UI
{
	class UIColors
	{
		public:
		UIColors(UIColors&) = delete;
		void operator=(const UIColors&) = delete;

		[[nodiscard]] static auto GetInstance() noexcept -> winrt::Windows::UI::ViewManagement::UISettings& { return uiSettings; }

		[[nodiscard]] static RGBA GetForegroundColor() noexcept;
		[[nodiscard]] static RGBA GetBackgroundColor() noexcept;
		[[nodiscard]] static RGBA GetAccentColor() noexcept;
		[[nodiscard]] static RGBA GetAccentDark1Color() noexcept;
		[[nodiscard]] static RGBA GetAccentDark2Color() noexcept;
		[[nodiscard]] static RGBA GetAccentDark3Color() noexcept;
		[[nodiscard]] static RGBA GetAccentLight1Color() noexcept;
		[[nodiscard]] static RGBA GetAccentLight2Color() noexcept;
		[[nodiscard]] static RGBA GetAccentLight3Color() noexcept;

		[[nodiscard]] static bool IsDarkMode() noexcept;
		[[nodiscard]] static bool IsLightMode() noexcept;

		protected:
		UIColors() = default;

		private:
		static inline winrt::Windows::UI::ViewManagement::UISettings uiSettings{ };
	};
}
