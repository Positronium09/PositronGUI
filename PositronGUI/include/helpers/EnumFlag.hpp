#pragma once

#include <type_traits>


namespace PGUI
{
	template<typename T> concept is_enum = std::is_enum_v<T>;
	template <typename T>
	concept is_enum_defined = requires
	{
		requires is_enum<typename T::EnumValues>;
	};
	template <typename T> using underlying_t = std::underlying_type_t<T>;

	template <is_enum_defined EnumDefType>
	class EnumFlag : public EnumDefType
	{
		private:
		using EnumType = EnumDefType::EnumValues;

		public:
		using UnderlyingType = underlying_t<EnumType>;

		constexpr explicit(false) EnumFlag(UnderlyingType _val) noexcept : val(static_cast<EnumType>(_val))
		{
		}
		constexpr ~EnumFlag() noexcept = default;

		constexpr explicit(false) operator UnderlyingType() const noexcept
		{
			return val;
		}

		constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(val);
		}

		constexpr bool operator!() const noexcept
		{
			return static_cast<bool>(static_cast<UnderlyingType>(val));
		}
		constexpr EnumFlag operator~() const noexcept
		{
			return ~static_cast<UnderlyingType>(val);
		}

		constexpr EnumFlag& operator&=(const EnumFlag& other) noexcept
		{
			val = static_cast<EnumType>(static_cast<UnderlyingType>(val) & static_cast<UnderlyingType>(other.val));
			return *this;
		}
		constexpr EnumFlag& operator|=(const EnumFlag& other) noexcept
		{
			val = static_cast<EnumType>(static_cast<UnderlyingType>(val) | static_cast<UnderlyingType>(other.val));
			return *this;
		}
		constexpr EnumFlag& operator^=(const EnumFlag& other) noexcept
		{
			val = static_cast<EnumType>(static_cast<UnderlyingType>(val) ^ static_cast<UnderlyingType>(other.val));
			return *this;
		}

		private:
		EnumType val;
	};
}
