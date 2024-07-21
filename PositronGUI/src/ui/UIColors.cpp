#include "ui/UIColors.hpp"

#include "core/Logger.hpp"
#include "core/Exceptions.hpp"

#include <windows.foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <objbase.h>


namespace PGUI::UI
{
	using enum winrt::Windows::UI::ViewManagement::UIColorType;

	RGBA UIColors::GetForegroundColor() noexcept
	{
		return uiSettings.GetColorValue(Foreground);
	}
	RGBA UIColors::GetBackgroundColor() noexcept
	{
		return uiSettings.GetColorValue(Background);
	}
	RGBA UIColors::GetAccentColor() noexcept
	{	
		return uiSettings.GetColorValue(Accent);
	}
	RGBA UIColors::GetAccentDark1Color() noexcept
	{	
		return uiSettings.GetColorValue(AccentDark1);
	}
	RGBA UIColors::GetAccentDark2Color() noexcept
	{	
		return uiSettings.GetColorValue(AccentDark2);
	}
	RGBA UIColors::GetAccentDark3Color() noexcept
	{
		return uiSettings.GetColorValue(AccentDark3);
	}
	RGBA UIColors::GetAccentLight1Color() noexcept
	{
		return uiSettings.GetColorValue(AccentLight1);
	}
	RGBA UIColors::GetAccentLight2Color() noexcept
	{
		return uiSettings.GetColorValue(AccentLight2);
	}
	RGBA UIColors::GetAccentLight3Color() noexcept
	{
		return uiSettings.GetColorValue(AccentLight3);
	}

	bool UIColors::IsDarkMode() noexcept
	{
		return !IsLightMode();
	}
	bool UIColors::IsLightMode() noexcept
	{
		DWORD value = 1;
		DWORD size = sizeof(value);

		if (LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, 
			LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
			L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &value, &size);
			status != ERROR_SUCCESS)
		{
			HR_L(HRESULT_FROM_WIN32(status));
		}

		return static_cast<bool>(value);
	}
}
