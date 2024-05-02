#include "ui/UIColors.hpp"

#include "core/Logger.hpp"
#include "core/Exceptions.hpp"

#include <windows.foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <objbase.h>


namespace PGUI::UI
{
	namespace abi_vm = ABI::Windows::UI::ViewManagement;
	namespace wrl = Microsoft::WRL;
	namespace wf = Windows::Foundation;

	template<typename ...Arg> std::unique_ptr<UIColors> static CreateUIColorsUniquePtr(Arg&&...arg)
	{
		struct EnableMakeShared : public UIColors
		{
			explicit(false) EnableMakeShared(Arg&&...arg) : UIColors(arg...) { }
		};
		return std::make_unique<EnableMakeShared>(std::forward<Arg>(arg)...);
	}

	UIColors* UIColors::GetInstance() noexcept
	{
		if (!instance)
		{
			instance = CreateUIColorsUniquePtr();

			ComPtr<abi_vm::IUISettings> settings;

			HRESULT hr = wf::ActivateInstance(wrl::Wrappers::HStringReference(
				RuntimeClass_Windows_UI_ViewManagement_UISettings).Get(), &settings); HR_T(hr);

			settings.As<abi_vm::IUISettings3>(&instance->uiSettings);
		}

		return instance.get();
	}

	RGBA UIColors::GetForegroundColor() const noexcept
	{
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_Foreground, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetBackgroundColor() const noexcept
	{
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_Background, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentColor() const noexcept
	{	
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_Accent, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentDark1Color() const noexcept
	{	
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentDark1, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentDark2Color() const noexcept
	{	
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentDark2, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentDark3Color() const noexcept
	{
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentDark3, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentLight1Color() const noexcept
	{
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentLight1, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentLight2Color() const noexcept
	{

		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentLight2, &color); HR_L(hr);

		return color;
	}
	RGBA UIColors::GetAccentLight3Color() const noexcept
	{
		ABI::Windows::UI::Color color;
		HRESULT hr = uiSettings->GetColorValue(
			ABI::Windows::UI::ViewManagement::UIColorType_AccentLight3, &color); HR_L(hr);

		return color;
	}

	bool UIColors::IsDarkMode() const noexcept
	{
		return !IsLightMode();
	}
	bool UIColors::IsLightMode() const noexcept
	{
		DWORD value = 1;
		DWORD size = sizeof(value);

		if (LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
			L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &value, &size);
			status != ERROR_SUCCESS)
		{
			HR_L(HRESULT_FROM_WIN32(status));
		}

		return static_cast<bool>(value);
	}
}
