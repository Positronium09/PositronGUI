#pragma once

#include "core/Point.hpp"
#include "core/Rect.hpp"
#include <functional>
#include <concepts>
#include <string>
#include <span>
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

	template <std::integral T>
	[[nodiscard]] constexpr int sign(T x) noexcept
	{
		if (x == 0)
		{
			return 0;
		}
		return x > 0 ? 1 : -1;
	}
	template <std::floating_point T>
	[[nodiscard]] constexpr int sign(T x) noexcept
	{
		if (x == 0)
		{
			return 0;
		}
		return std::signbit(x) ? -1 : 1;
	}

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

	[[nodiscard]] int AdjustForDpi(int value, int dpi) noexcept;

	[[nodiscard]] std::span<PointL> MapPoints(HWND from, HWND to, std::span<PointL> points) noexcept;
	[[nodiscard]] PointL MapPoint(HWND from, HWND to, PointL point) noexcept;

	[[nodiscard]] std::span<RectL> MapRects(HWND from, HWND to, std::span<RectL> rects) noexcept;
	[[nodiscard]] RectL MapRect(HWND from, HWND to, RectL rect) noexcept;
}
