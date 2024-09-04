#include "ui/UIColors.hpp"

#include "helpers/HelperFunctions.hpp"
#include "core/Exceptions.hpp"

#include <windows.foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <objbase.h>


namespace PGUI::UI
{
	using enum winrt::Windows::UI::ViewManagement::UIColorType;

	auto UIColors::GetForegroundColor() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(Foreground);
	}
	auto UIColors::GetBackgroundColor() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(Background);
	}
	auto UIColors::GetAccentColor() noexcept -> RGBA
	{	
		return uiSettings.GetColorValue(Accent);
	}
	auto UIColors::GetAccentDark1Color() noexcept -> RGBA
	{	
		return uiSettings.GetColorValue(AccentDark1);
	}
	auto UIColors::GetAccentDark2Color() noexcept -> RGBA
	{	
		return uiSettings.GetColorValue(AccentDark2);
	}
	auto UIColors::GetAccentDark3Color() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(AccentDark3);
	}
	auto UIColors::GetAccentLight1Color() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(AccentLight1);
	}
	auto UIColors::GetAccentLight2Color() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(AccentLight2);
	}
	auto UIColors::GetAccentLight3Color() noexcept -> RGBA
	{
		return uiSettings.GetColorValue(AccentLight3);
	}

	auto UIColors::IsDarkMode() noexcept -> bool
	{
		return !IsLightMode();
	}
	auto UIColors::IsLightMode() noexcept -> bool
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
