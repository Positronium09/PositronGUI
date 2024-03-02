#pragma once

#include "Point.hpp"
#include "Size.hpp"

#include <cstdint>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>


template <typename T> concept _rc_arithmetic = std::is_arithmetic_v<T>;

namespace PGUI
{
	template<_rc_arithmetic T>
	struct Rect
	{
		T left = (T)0;
		T top = (T)0;
		T right = (T)0;
		T bottom = (T)0;

		constexpr Rect() = default;
		constexpr Rect(T left_, T top_, T right_, T bottom_) :
			left(left_), top(top_), right(right_), bottom(bottom_)
		{
		}
		explicit(false) constexpr Rect(const RECT& rc) :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		explicit(false) constexpr Rect(const D2D1_RECT_F& rc) :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		explicit(false) constexpr Rect(const D2D1_RECT_U& rc) :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		~Rect() = default;

		constexpr bool operator==(const Rect<T>& other) const = default;

		[[nodiscard]] constexpr Point<T> TopLeft() const
		{
			return { left, top };
		}
		[[nodiscard]] constexpr Point<T> BottomRight() const
		{
			return { right, bottom };
		}

		[[nodiscard]] constexpr T Width() const
		{
			return right - left;
		}
		[[nodiscard]] constexpr T Height() const
		{
			return bottom - top;
		}

		[[nodiscard]] constexpr Size<T> Size() const
		{
			return { Width(), Height() };
		}

		[[nodiscard]] constexpr bool IsPointInside(Point<T> p) const
		{
			return 
				left <= p.x && p.x <= right && 
				top <= p.y && p.y <= bottom;
		}

		constexpr void Shift(T xOffset, T yOffset)
		{
			left += xOffset;
			right += xOffset;
			top += yOffset;
			bottom += yOffset;
		}

		[[nodiscard]] constexpr Rect<T> Shifted(T xOffset, T yOffset) const
		{
			auto rect = *this;
			rect.Shift(xOffset, yOffset);
			return rect;
		}

		[[nodiscard]] constexpr Point<T> Center() const
		{
			return Point<T>((left + right) / 2, (top + bottom) / 2);
		}

		constexpr void CenterAround(Point<T> p)
		{
			auto size = Size() / static_cast<T>(2);

			left = p.x - size.cx;
			right = p.x + size.cx;
			top = p.y + size.cy;
			bottom = p.y - size.cy;
		}
		[[nodiscard]] constexpr Rect<T> CenteredAround(Point<T> p) const
		{
			auto size = Size() / static_cast<T>(2);

			return Rect<T>{
				p.x - size.cx, p.y + size.cy,
				p.x + size.cx, p.y - size.cy
			};
		}

		template<typename U>
		explicit operator Rect<U>() const
		{
			return Rect<U>{
				static_cast<U>(left), static_cast<U>(top),
				static_cast<U>(right), static_cast<U>(bottom) };
		}

		explicit(false) operator RECT() const
		{
			return RECT{
				static_cast<LONG>(left), static_cast<LONG>(top),
				static_cast<LONG>(right), static_cast<LONG>(bottom) };
		}
		explicit(false) operator D2D1_RECT_F() const
		{
			return D2D1_RECT_F{
				static_cast<FLOAT>(left), static_cast<FLOAT>(top),
				static_cast<FLOAT>(right), static_cast<FLOAT>(bottom) };
		}
		explicit(false) operator D2D1_RECT_U() const
		{
			return D2D1_RECT_U{
				static_cast<UINT32>(left), static_cast<UINT32>(top),
				static_cast<UINT32>(right), static_cast<UINT32>(bottom) };
		}
	};

	using RectF = Rect<float>;
	using RectI = Rect<int>;
	using RectL = Rect<long>;
	using RectU = Rect<std::uint32_t>;
}
