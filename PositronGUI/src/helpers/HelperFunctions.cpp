#include "helpers/HelperFunctions.hpp"

#include "core/Logger.hpp"
#include "ui/Brush.hpp"
#include "ui/Gradient.hpp"

#include <winrt/windows.globalization.h>

#include <bit>
#include <cstring>
#include <dwmapi.h>
#include <Shlwapi.h>
#include <system_error>


namespace PGUI
{
	auto StringToWString(std::string_view string) noexcept -> std::wstring
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

	auto WStringToString(std::wstring_view string) noexcept -> std::string
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

	auto GetHInstance() noexcept -> HINSTANCE
	{
		return GetModuleHandleW(nullptr);
	}

	auto GetUserLocaleName() noexcept -> std::wstring
	{
		std::wstring localeName(LOCALE_NAME_MAX_LENGTH, '\0');

		GetUserDefaultLocaleName(localeName.data(), LOCALE_NAME_MAX_LENGTH);

		localeName.shrink_to_fit();

		return localeName;
	}

	auto GetCurrentInputMethodLanguage() noexcept -> std::wstring
	{
		using winrt::Windows::Globalization::Language;

		std::wstring localeName(LOCALE_NAME_MAX_LENGTH, L'\0');
		if (auto ret = ResolveLocaleName(
			Language::CurrentInputMethodLanguageTag().c_str(),
			localeName.data(), LOCALE_NAME_MAX_LENGTH);
			ret == 0)
		{
			HR_L(HresultFromWin32());
		}

		return localeName;
	}

	void EnableDarkTitleBar(HWND hWnd) noexcept
	{
		BOOL val = true;
		HRESULT hr = DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &val, sizeof(BOOL)); HR_L(hr);
		hr = DwmSetWindowAttribute(hWnd, 19, &val, sizeof(BOOL)); HR_L(hr);
	}

	auto GetHresultErrorMessage(HRESULT hResult) noexcept -> std::wstring
	{
		return StringToWString(std::system_category().message(hResult));
	}

	auto GetWin32ErrorMessage() noexcept -> std::wstring
	{
		return StringToWString(std::system_category().message(HresultFromWin32()));
	}
	auto GetWin32ErrorMessage(DWORD errorCode) noexcept -> std::wstring
	{
		return StringToWString(std::system_category().message(HRESULT_FROM_WIN32(errorCode)));
	}

	auto MapPoints(HWND from, HWND to, std::span<PointL> points) noexcept -> std::span<PointL>
	{
		MapWindowPoints(from, to,
			std::bit_cast<LPPOINT>(points.data()),
			static_cast<UINT>(points.size()));

		return points;
	}

	auto MapPoint(HWND from, HWND to, PointL point) noexcept -> PointL
	{
		MapWindowPoints(from, to,
			std::bit_cast<LPPOINT>(&point), 1U);

		return point;
	}

	auto MapRects(HWND from, HWND to, std::span<RectL> rects) noexcept -> std::span<RectL>
	{
		MapWindowPoints(from, to,
			std::bit_cast<LPPOINT>(rects.data()),
			static_cast<UINT>(rects.size()) * 2);

		return rects;
	}

	auto MapRect(HWND from, HWND to, RectL rect) noexcept -> RectL
	{
		MapWindowPoints(from, to,
			std::bit_cast<LPPOINT>(&rect), 2U);

		return rect;
	}

	void HR_L(HRESULT hr, std::source_location location) noexcept
	{
		if (FAILED(hr))
		{
			using UHRESULT = std::make_unsigned_t<HRESULT>;
			auto* filename = PathFindFileNameA(location.file_name());
			auto part1 = std::format("In File {}", filename);
			auto part2 = std::format("In Function {} at line {}",
				location.function_name(), location.line()
			);
			auto part3 = std::format(L"Code 0x{:X} {}",
				static_cast<UHRESULT>(hr), GetHresultErrorMessage(hr)
			);

			PGUI::Core::Logger::Error(StringToWString(part1));
			PGUI::Core::Logger::Error(StringToWString(part2));
			PGUI::Core::Logger::Error(part3);
		}
	}

	void HR_L(const PGUI::Core::HresultException& hrException, std::source_location location) noexcept
	{
		HR_L(hrException.GetErrorCode(), location);
	}

	void HR_T(HRESULT hr, std::source_location location) noexcept(false)
	{
		if (FAILED(hr))
		{
			HR_L(hr, location);
			throw PGUI::Core::HresultException{ hr };
		}
	}
}
