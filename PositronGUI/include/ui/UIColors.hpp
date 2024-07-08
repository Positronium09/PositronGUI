#pragma once

#include "helpers/ComPtr.hpp"
#include "ui/Color.hpp"

#include <memory>
#include <windows.ui.viewmanagement.h>



namespace PGUI::UI
{
	class UIColors
	{
		public:
		UIColors(UIColors&) = delete;
		void operator=(const UIColors&) = delete;

		static [[nodiscard]] UIColors* GetInstance() noexcept;

		[[nodiscard]] RGBA GetForegroundColor() const noexcept;
		[[nodiscard]] RGBA GetBackgroundColor() const noexcept;
		[[nodiscard]] RGBA GetAccentColor() const noexcept;
		[[nodiscard]] RGBA GetAccentDark1Color() const noexcept;
		[[nodiscard]] RGBA GetAccentDark2Color() const noexcept;
		[[nodiscard]] RGBA GetAccentDark3Color() const noexcept;
		[[nodiscard]] RGBA GetAccentLight1Color() const noexcept;
		[[nodiscard]] RGBA GetAccentLight2Color() const noexcept;
		[[nodiscard]] RGBA GetAccentLight3Color() const noexcept;

		[[nodiscard]] bool IsDarkMode() const noexcept;
		[[nodiscard]] bool IsLightMode() const noexcept;

		protected:
		UIColors() = default;

		private:
		ComPtr<ABI::Windows::UI::ViewManagement::IUISettings3> uiSettings;

		static inline std::unique_ptr<UIColors> instance = nullptr;
	};
}
