#pragma once

#include <functional>
#include <concepts>
#include <string>
#include <Windows.h>


namespace PGUI
{
	[[nodiscard]] std::wstring StringToWString(std::string_view string) noexcept;
	[[nodiscard]] std::string WStringToString(std::wstring_view string) noexcept;

	[[nodiscard]] HINSTANCE GetHInstance() noexcept;

	[[nodiscard]] std::wstring GetUserLocaleName() noexcept;

	void EnableDarkTitleBar(HWND hWnd) noexcept;

	[[nodiscard]] std::wstring GetHresultErrorMessage(HRESULT hResult) noexcept;
	[[nodiscard]] std::wstring GetWin32ErrorMessage() noexcept;
	[[nodiscard]] std::wstring GetWin32ErrorMessage(DWORD errorCode) noexcept;

	template<std::floating_point T>
	[[nodiscard]] constexpr T MapToRange(T value,
		T outRangeMin, T outRangeMax, T inRangeMin = 0, T inRangeMax = 1) noexcept
	{
		return outRangeMin + (outRangeMax - outRangeMin) * (value - inRangeMin) / (inRangeMax - inRangeMin);
	}

	template <typename T>
	[[nodiscard]] size_t GetTypeHash()
	{
		return typeid(T).hash_code();
	}

	template <typename T, typename... Args>
	[[nodiscard]] constexpr std::function<void(Args...)> BindMemberFunc(void (T::* memberFunc)(Args...), T* ptr) noexcept
	{
		return std::bind_front(memberFunc, ptr);
	}
}
