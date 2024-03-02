#include "helpers/StringConversions.hpp"

#include <Windows.h>


namespace PGUI
{
	std::wstring PGUI::StringToWString(std::string_view string)
	{
		if (string.empty())
		{
			return std::wstring{ };
		}

		int size = MultiByteToWideChar(CP_UTF8, NULL, string.data(), static_cast<int>(string.size()), nullptr, NULL);
		std::wstring converted(size, 0);

		MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), converted.data(), size);
		
		return converted;
	}

	std::string WStringToString(std::wstring_view string)
	{
		if (string.empty())
		{
			return std::string{ };
		}

		int size = WideCharToMultiByte(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0, nullptr, nullptr);
		std::string converted(size, 0);		

		WideCharToMultiByte(CP_UTF8, 0, string.data(), (int)string.size(), converted.data(), size, nullptr, nullptr);

		return converted;
	}
}
