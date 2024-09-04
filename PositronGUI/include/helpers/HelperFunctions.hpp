#pragma once

#include "core/Point.hpp"
#include "core/Rect.hpp"
#include "core/Exceptions.hpp"
#include "core/Logger.hpp"
#include <functional>
#include <concepts>
#include <string>
#include <span>
#include <format>
#include <source_location>
#include <Windows.h>


namespace PGUI
{
	[[nodiscard]] auto StringToWString(std::string_view string) noexcept -> std::wstring;
	[[nodiscard]] auto WStringToString(std::wstring_view string) noexcept -> std::string;

	[[nodiscard]] auto GetHInstance() noexcept -> HINSTANCE;

	[[nodiscard]] auto GetUserLocaleName() noexcept -> std::wstring;
	[[nodiscard]] auto GetCurrentInputMethodLanguage() noexcept -> std::wstring;

	void EnableDarkTitleBar(HWND hWnd) noexcept;

	[[nodiscard]] auto GetHresultErrorMessage(HRESULT hResult) noexcept -> std::wstring;
	[[nodiscard]] auto GetWin32ErrorMessage() noexcept -> std::wstring;
	[[nodiscard]] auto GetWin32ErrorMessage(DWORD errorCode) noexcept -> std::wstring;

	template <std::integral T>
	[[nodiscard]] constexpr auto sign(T x) noexcept -> int
	{
		if (x == 0)
		{
			return 0;
		}
		return x > 0 ? 1 : -1;
	}
	template <std::floating_point T>
	[[nodiscard]] constexpr auto sign(T x) noexcept -> int
	{
		if (x == 0)
		{
			return 0;
		}
		return std::signbit(x) ? -1 : 1;
	}

	template<std::floating_point T>
	[[nodiscard]] constexpr auto MapToRange(T value,
		T outRangeMin, T outRangeMax, T inRangeMin = 0, T inRangeMax = 1) noexcept -> T
	{
		return outRangeMin + (outRangeMax - outRangeMin) * (value - inRangeMin) / (inRangeMax - inRangeMin);
	}

	template <typename T>
	[[nodiscard]] auto GetTypeHash() noexcept -> size_t
	{
		return typeid(T).hash_code();
	}

	template <typename T, typename... Args>
	[[nodiscard]] constexpr auto BindMemberFunc(void (T::* memberFunc)(Args...), T* ptr) noexcept -> std::function<void(Args...)>
	{
		return std::bind_front(memberFunc, ptr);
	}

	template <typename T, std::floating_point U>
	[[nodiscard]] auto AdjustForDPI(T value, U dpi) noexcept -> T
	{
		return static_cast<T>(value * dpi / static_cast<U>(96.0));
	}
	template <typename T, std::floating_point U>
	[[nodiscard]] auto ScaleForDPI(T value, U dpiScale) noexcept -> T
	{
		return static_cast<T>(value * dpiScale);
	}
	template <typename T, std::floating_point U>
	[[nodiscard]] auto RemoveDPIAdjustment(T value, U dpi) noexcept -> T
	{
		return static_cast<T>(value * static_cast<U>(96.0) / dpi);
	}
	template <typename T, std::floating_point U>
	[[nodiscard]] auto RemoveDPIScale(T value, U dpiScale) noexcept -> T
	{
		return static_cast<T>(value / dpiScale);
	}

	[[nodiscard]] auto MapPoints(HWND from, HWND to, std::span<PointL> points) noexcept -> std::span<PointL>;
	[[nodiscard]] auto MapPoint(HWND from, HWND to, PointL point) noexcept -> PointL;

	[[nodiscard]] auto MapRects(HWND from, HWND to, std::span<RectL> rects) noexcept -> std::span<RectL>;
	[[nodiscard]] auto MapRect(HWND from, HWND to, RectL rect) noexcept -> RectL;

	void HR_L(HRESULT hr,
		std::source_location location = std::source_location::current()) noexcept;
	void HR_L(const PGUI::Core::HresultException& hrException,
		std::source_location location = std::source_location::current()) noexcept;

	void HR_T(HRESULT hr,
		std::source_location location = std::source_location::current()) noexcept(false);

	[[nodiscard]] static inline auto HresultFromWin32() noexcept -> HRESULT
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	[[nodiscard]] static inline auto HresultFromWin32(DWORD errCode) noexcept -> HRESULT
	{
		return HRESULT_FROM_WIN32(errCode);
	}
}
