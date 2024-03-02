#pragma once

#include <type_traits>


namespace PGUI
{
	template<typename T> concept is_enum = std::is_enum_v<T>;

	template<is_enum T> inline std::underlying_type_t<T> operator~ (T a) { return ~(std::underlying_type_t<T>)a; }
	template<is_enum T> inline std::underlying_type_t<T> operator| (T a, T b) { return (std::underlying_type_t<T>)a | (std::underlying_type_t<T>)b; }
	template<is_enum T> inline std::underlying_type_t<T> operator& (T a, T b) { return (std::underlying_type_t<T>)a & (std::underlying_type_t<T>)b; }
	template<is_enum T> inline std::underlying_type_t<T> operator^ (T a, T b) { return (std::underlying_type_t<T>)a ^ (std::underlying_type_t<T>)b; }
	template<is_enum T> inline T& operator|= (T& a, T b) { return (T&)((std::underlying_type_t<T>&)a |= (std::underlying_type_t<T>)b); }
	template<is_enum T> inline T& operator&= (T& a, T b) { return (T&)((std::underlying_type_t<T>&)a &= (std::underlying_type_t<T>)b); }
	template<is_enum T> inline T& operator^= (T& a, T b) { return (T&)((std::underlying_type_t<T>&)a ^= (std::underlying_type_t<T>)b); }
}
