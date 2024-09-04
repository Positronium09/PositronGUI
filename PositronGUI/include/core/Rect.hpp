#pragma once

#include "Point.hpp"
#include "Size.hpp"


#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>

#undef min
#undef max


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

		constexpr Rect() noexcept = default;
		constexpr Rect(T left_, T top_, T right_, T bottom_) noexcept :
			left(left_), top(top_), right(right_), bottom(bottom_)
		{
		}
		explicit(false) constexpr Rect(const RECT& rc) noexcept :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		explicit(false) constexpr Rect(const D2D1_RECT_F& rc) noexcept :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		explicit(false) constexpr Rect(const D2D1_RECT_U& rc) noexcept :
			left((T)rc.left), top((T)rc.top), right((T)rc.right), bottom((T)rc.bottom)
		{
		}
		constexpr Rect(Point<T> position, Size<T> size) noexcept : 
			left(position.x), top(position.y), right(position.x + size.cx), bottom(position.y + size.cy)
		{
		}
		~Rect() noexcept = default;

		[[nodiscard]] constexpr bool operator==(const Rect<T>& other) const noexcept = default;

		[[nodiscard]] constexpr Point<T> TopLeft() const noexcept
		{
			return { left, top };
		}
		[[nodiscard]] constexpr Point<T> BottomLeft() const noexcept
		{
			return { left, bottom };
		}
		[[nodiscard]] constexpr Point<T> TopRight() const noexcept
		{
			return { right, top };
		}
		[[nodiscard]] constexpr Point<T> BottomRight() const noexcept
		{
			return { right, bottom };
		}

		[[nodiscard]] constexpr T Width() const noexcept
		{
			return right - left;
		}
		[[nodiscard]] constexpr T Height() const noexcept
		{
			return bottom - top;
		}

		[[nodiscard]] constexpr Size<T> Size() const noexcept
		{
			return { Width(), Height() };
		}

		[[nodiscard]] constexpr bool IsPointInside(Point<T> p) const noexcept
		{
			return
				left <= p.x && p.x <= right &&
				top <= p.y && p.y <= bottom;
		}

		[[nodiscard]] constexpr bool IsIntersectingRect(Rect<T> rect) const noexcept
		{
			return left < rect.right
				&& right > rect.left
				&& top < rect.bottom
				&& bottom > top;
		}
		[[nodiscard]] constexpr Rect<T> IntersectRect(Rect<T> rect) const noexcept
		{
			if (!IsIntersectingRect(rect))
			{
				return Rect<T>{ };
			}
			return Rect<T>{
				std::max(left, rect.left),
				std::max(top, rect.top),
				std::min(right, rect.right),
				std::min(bottom, rect.bottom)
			};
		}

		constexpr T Area() const noexcept
		{
			auto size = Size();
			return size.cx * size.cy;
		}

		constexpr void Shift(T xOffset, T yOffset) noexcept
		{
			left += xOffset;
			right += xOffset;
			top += yOffset;
			bottom += yOffset;
		}
		[[nodiscard]] constexpr Rect<T> Shifted(T xOffset, T yOffset) const noexcept
		{
			auto rect = *this;
			rect.Shift(xOffset, yOffset);
			return rect;
		}

		constexpr void Inflate(T dx, T dy) noexcept
		{
			left += -dx;
			right += dx;
			top -= dy;
			bottom += dy;
		}
		[[nodiscard]] constexpr Rect<T> Inflated(T dx, T dy) const noexcept
		{
			auto rect = *this;
			rect.Inflate(dx, dy);
			return rect;
		}

		[[nodiscard]] constexpr Point<T> Center() const noexcept
		{
			return Point<T>((left + right) / 2, (top + bottom) / 2);
		}

		constexpr void CenterAround(Point<T> p) noexcept
		{
			auto size = Size() / static_cast<T>(2);

			left = p.x - size.cx;
			right = p.x + size.cx;
			top = p.y - size.cy;
			bottom = p.y + size.cy;
		}
		[[nodiscard]] constexpr Rect<T> CenteredAround(Point<T> p) const noexcept
		{
			auto size = Size() / static_cast<T>(2);

			return Rect<T>{
				p.x - size.cx, p.y + size.cy,
					p.x + size.cx, p.y - size.cy
			};
		}

		template<typename U>
		explicit(false) constexpr operator Rect<U>() const noexcept
		{
			return Rect<U>{
				static_cast<U>(left), static_cast<U>(top),
					static_cast<U>(right), static_cast<U>(bottom) };
		}

		explicit(false) constexpr operator RECT() const noexcept
		{
			return RECT{
				static_cast<LONG>(left), static_cast<LONG>(top),
				static_cast<LONG>(right), static_cast<LONG>(bottom) };
		}
		explicit(false) constexpr operator D2D1_RECT_F() const noexcept
		{
			return D2D1_RECT_F{
				static_cast<FLOAT>(left), static_cast<FLOAT>(top),
				static_cast<FLOAT>(right), static_cast<FLOAT>(bottom) };
		}
		explicit(false) constexpr operator D2D1_RECT_U() const noexcept
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
