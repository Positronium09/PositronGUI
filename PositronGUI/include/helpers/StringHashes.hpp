#pragma once

#include <string_view>


namespace PGUI
{
	struct StringHash
	{
		using is_transparent = void;

		auto operator()(std::string_view sv) const
		{
			std::hash<std::string_view> hasher;
			return hasher(sv);
		}
	};
	struct WStringHash
	{
		using is_transparent = void;

		auto operator()(std::wstring_view sv) const
		{
			std::hash<std::wstring_view> hasher;
			return hasher(sv);
		}
	};
}
