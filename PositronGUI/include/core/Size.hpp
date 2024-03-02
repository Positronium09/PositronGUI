#pragma once

#include <cstdint>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>


template <typename T> concept _sz_arithmetic = std::is_arithmetic_v<T>;

namespace PGUI
{
	template <_sz_arithmetic T>
	struct Size
	{
		T cx;
		T cy;

		constexpr Size() = default;
		constexpr Size(const T& cx_, const T& cy_) :
			cx{ cx_ }, cy{ cy_ }
		{
		}
		explicit(false) constexpr Size(const SIZE& sz) :
			cx{ (T)sz.cx }, cy{ (T)sz.cy }
		{
		}
		explicit(false) constexpr Size(const D2D1_SIZE_F& sz) :
			cx{ (T)sz.width }, cy{ (T)sz.height }
		{
		}
		explicit(false) constexpr Size(const D2D1_SIZE_U& sz) :
			cx{ (T)sz.width }, cy{ (T)sz.height }
		{
		}
		~Size() = default;

		constexpr bool operator==(const Size<T>& other) const = default;

		constexpr Size& operator*=(const T& factor)
		{
			cx *= factor;
			cx *= factor;
			return *this;
		}
		constexpr Size& operator/=(const T& factor)
		{
			cx /= factor;
			cx /= factor;
			return *this;
		}

		constexpr Size operator*(T factor) const
		{
			return Size(cx * factor, cy * factor);
		}
		constexpr Size operator/(T factor) const
		{
			return Size(cx / factor, cy / factor);
		}

		template<typename U>
		explicit operator Size<U>() const
		{
			return Size<U>{ static_cast<U>(cx), static_cast<U>(cy) };
		}

		explicit(false) operator SIZE() const
		{
			return SIZE{ static_cast<LONG>(cx), static_cast<LONG>(cy) };
		}
		explicit(false) operator D2D1_SIZE_F() const
		{
			return D2D1_SIZE_F{ static_cast<FLOAT>(cx), static_cast<FLOAT>(cy) };
		}
		explicit(false) operator D2D1_SIZE_U() const
		{
			return D2D1_SIZE_U{ static_cast<UINT32>(cx), static_cast<UINT32>(cy) };
		}
	};


	using SizeF = Size<float>;
	using SizeI = Size<int>;
	using SizeL = Size<long>;
	using SizeU = Size<std::uint32_t>;
}