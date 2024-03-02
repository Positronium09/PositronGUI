#include "helpers/HelperFunctions.hpp"

#include <system_error>
#include <dwmapi.h>


HINSTANCE GetHInstance()
{
	return GetModuleHandleW(nullptr);
}

std::wstring GetUserLocaleName()
{
	std::wstring localeName{ };
	localeName.reserve(LOCALE_NAME_MAX_LENGTH);

	GetUserDefaultLocaleName(localeName.data(), LOCALE_NAME_MAX_LENGTH);

	return localeName;
}

void EnableDarkTitleBar(HWND hWnd)
{
	BOOL val = true;
	DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &val, sizeof(BOOL));
	DwmSetWindowAttribute(hWnd, 19, &val, sizeof(BOOL));
}

std::string GetHresultErrorMessage(HRESULT hResult)
{
	return std::system_category().message(hResult);
}

std::string GetWin32ErrorMessage()
{
	return std::system_category().message(HRESULT_FROM_WIN32(GetLastError()));
}
std::string GetWin32ErrorMessage(DWORD errorCode)
{
	return std::system_category().message(HRESULT_FROM_WIN32(errorCode));
}
