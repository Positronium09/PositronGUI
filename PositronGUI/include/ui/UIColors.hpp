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

		[[nodiscard]] static auto GetForegroundColor() noexcept -> RGBA;
		[[nodiscard]] static auto GetBackgroundColor() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentColor() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentDark1Color() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentDark2Color() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentDark3Color() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentLight1Color() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentLight2Color() noexcept -> RGBA;
		[[nodiscard]] static auto GetAccentLight3Color() noexcept -> RGBA;

		[[nodiscard]] static auto IsDarkMode() noexcept -> bool;
		[[nodiscard]] static auto IsLightMode() noexcept -> bool;

		protected:
		UIColors() = default;

		private:
		static inline winrt::Windows::UI::ViewManagement::UISettings uiSettings{ };
	};
}
