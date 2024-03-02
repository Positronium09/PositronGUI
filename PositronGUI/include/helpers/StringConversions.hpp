#pragma once

#include <string>


namespace PGUI
{
	[[nodiscard]] std::wstring StringToWString(std::string_view string);
	[[nodiscard]] std::string WStringToString(std::wstring_view string);
}
