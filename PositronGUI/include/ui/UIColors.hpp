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

		static UIColors* GetInstance() noexcept;

		RGBA GetForegroundColor() const noexcept;
		RGBA GetBackgroundColor() const noexcept;
		RGBA GetAccentColor() const noexcept;
		RGBA GetAccentDark1Color() const noexcept;
		RGBA GetAccentDark2Color() const noexcept;
		RGBA GetAccentDark3Color() const noexcept;
		RGBA GetAccentLight1Color() const noexcept;
		RGBA GetAccentLight2Color() const noexcept;
		RGBA GetAccentLight3Color() const noexcept;

		bool IsDarkMode() const noexcept;
		bool IsLightMode() const noexcept;

		protected:
		UIColors() = default;

		private:
		ComPtr<ABI::Windows::UI::ViewManagement::IUISettings3> uiSettings;

		static inline std::unique_ptr<UIColors> instance = nullptr;
	};
}
