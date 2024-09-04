#pragma once

#include <concepts>


//! macro scary :O
#define EnableEnumFlag(T) namespace enum_flag_detail { template<> struct enum_flag_enable<T> : public std::true_type{}; }

template<typename T>
concept enumeration = std::is_enum_v<T>;

namespace enum_flag_detail
{
	template <enumeration T>
	struct enum_flag_enable : public std::false_type { };
}


template <typename T>
concept enum_flag = enumeration<T> && enum_flag_detail::enum_flag_enable<T>::value;
template <enumeration T>
using underlying_t = std::underlying_type_t<T>;

template <
	enum_flag EnumFlagType,
	typename UnderlyingType = underlying_t<EnumFlagType>>
	constexpr EnumFlagType operator|(EnumFlagType lhs, EnumFlagType rhs) noexcept
{
	return static_cast<EnumFlagType>(
		static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs)
		);
}
constexpr auto& operator|=(enum_flag auto& lhs, enum_flag auto rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}
template <
	enum_flag EnumFlagType,
	typename UnderlyingType = underlying_t<EnumFlagType>>
	constexpr EnumFlagType operator&(EnumFlagType lhs, EnumFlagType rhs) noexcept
{
	return static_cast<EnumFlagType>(
		static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs)
		);
}
constexpr auto& operator&=(enum_flag auto& lhs, enum_flag auto rhs) noexcept
{
	lhs = lhs & rhs;
	return lhs;
}
template <
	enum_flag EnumFlagType,
	typename UnderlyingType = underlying_t<EnumFlagType>>
	constexpr EnumFlagType operator^(EnumFlagType lhs, EnumFlagType rhs) noexcept
{
	return static_cast<EnumFlagType>(
		static_cast<UnderlyingType>(lhs) ^ static_cast<UnderlyingType>(rhs)
		);
}
constexpr auto& operator^=(enum_flag auto& lhs, enum_flag auto rhs) noexcept
{
	lhs = lhs ^ rhs;
	return lhs;
}
template <
	enum_flag EnumFlagType,
	typename UnderlyingType = underlying_t<EnumFlagType>>
	constexpr EnumFlagType operator~(EnumFlagType lhs) noexcept
{
	return static_cast<EnumFlagType>(
		~static_cast<UnderlyingType>(lhs));
}
template <
	enum_flag EnumFlagType,
	typename UnderlyingType = underlying_t<EnumFlagType>>
	constexpr bool operator!(EnumFlagType lhs)
{
	return !static_cast<UnderlyingType>(lhs);
}
template <enum_flag EnumFlagType>
bool IsFlagSet(EnumFlagType var, EnumFlagType flag)
{
	return (var & flag) != static_cast<EnumFlagType>(0);
}
