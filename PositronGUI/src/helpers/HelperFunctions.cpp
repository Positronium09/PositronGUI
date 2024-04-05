#include "helpers/HelperFunctions.hpp"

#include "ui/Brush.hpp"
#include "ui/Gradient.hpp"

#include <string.h>
#include <system_error>
#include <dwmapi.h>


namespace PGUI
{
	std::wstring PGUI::StringToWString(std::string_view string) noexcept
	{
		if (string.empty())
		{
			return std::wstring{ };
		}

		int size = MultiByteToWideChar(CP_UTF8, NULL, string.data(), static_cast<int>(string.size()), nullptr, NULL);
		std::wstring converted(size, L'\0');

		MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), converted.data(), size);

		return converted;
	}

	std::string WStringToString(std::wstring_view string) noexcept
	{
		if (string.empty())
		{
			return std::string{ };
		}

		int size = WideCharToMultiByte(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0, nullptr, nullptr);
		std::string converted(size, '\0');

		WideCharToMultiByte(CP_UTF8, 0, string.data(), (int)string.size(), converted.data(), size, nullptr, nullptr);

		return converted;
	}

	HINSTANCE GetHInstance() noexcept
	{
		return GetModuleHandleW(nullptr);
	}

	std::wstring GetUserLocaleName() noexcept
	{
		std::wstring localeName(LOCALE_NAME_MAX_LENGTH, '\0');

		GetUserDefaultLocaleName(localeName.data(), LOCALE_NAME_MAX_LENGTH);

		localeName.shrink_to_fit();

		return localeName;
	}

	void EnableDarkTitleBar(HWND hWnd) noexcept
	{
		BOOL val = true;
		DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &val, sizeof(BOOL));
		DwmSetWindowAttribute(hWnd, 19, &val, sizeof(BOOL));
	}

	std::wstring GetHresultErrorMessage(HRESULT hResult) noexcept
	{
		return StringToWString(std::system_category().message(hResult));
	}

	std::wstring GetWin32ErrorMessage() noexcept
	{
		return StringToWString(std::system_category().message(HRESULT_FROM_WIN32(GetLastError())));
	}
	std::wstring GetWin32ErrorMessage(DWORD errorCode) noexcept
	{
		return StringToWString(std::system_category().message(HRESULT_FROM_WIN32(errorCode)));
	}
}
