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

		static [[nodiscard]] auto GetInstance() noexcept -> winrt::Windows::UI::ViewManagement::UISettings& { return uiSettings; }

		static [[nodiscard]] RGBA GetForegroundColor() noexcept;
		static [[nodiscard]] RGBA GetBackgroundColor() noexcept;
		static [[nodiscard]] RGBA GetAccentColor() noexcept;
		static [[nodiscard]] RGBA GetAccentDark1Color() noexcept;
		static [[nodiscard]] RGBA GetAccentDark2Color() noexcept;
		static [[nodiscard]] RGBA GetAccentDark3Color() noexcept;
		static [[nodiscard]] RGBA GetAccentLight1Color() noexcept;
		static [[nodiscard]] RGBA GetAccentLight2Color() noexcept;
		static [[nodiscard]] RGBA GetAccentLight3Color() noexcept;

		static [[nodiscard]] bool IsDarkMode() noexcept;
		static [[nodiscard]] bool IsLightMode() noexcept;

		protected:
		UIColors() = default;

		private:
		static inline winrt::Windows::UI::ViewManagement::UISettings uiSettings{ };
	};
}
