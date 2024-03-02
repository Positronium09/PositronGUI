#pragma once

#include <functional>
#include <string>
#include <Windows.h>


[[nodiscard]] HINSTANCE GetHInstance();

[[nodiscard]] std::wstring GetUserLocaleName();

void EnableDarkTitleBar(HWND hWnd);

[[nodiscard]] std::string GetHresultErrorMessage(HRESULT hResult);
[[nodiscard]] std::string GetWin32ErrorMessage();
[[nodiscard]] std::string GetWin32ErrorMessage(DWORD errorCode);

template <typename T, typename... Args>
[[nodiscard]] std::function<void(Args...)> BindMemberFunc(void (T::*memberFunc)(Args...), T* ptr)
{
	return std::bind_front(memberFunc, ptr);
}
