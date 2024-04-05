#pragma once

#include <cmath>
#include <cstdint>
#include <numbers>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>


template <typename T> concept _pt_arithmetic = std::is_arithmetic_v<T>;

namespace PGUI
{
	template<_pt_arithmetic T>
	struct Point
	{
		T x = (T)0;
		T y = (T)0;

		[[nodiscard]] static T Distance(Point<T> A, Point<T> B) noexcept
		{
			return std::hypot(A.x - B.x, A.y - B.y);
		}

		constexpr Point() noexcept = default;
		constexpr Point(const T& x_, const T& y_) noexcept :
			x{ x_ }, y{ y_ }
		{
		}
		explicit(false) constexpr Point(const POINT& p) noexcept :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const POINTS& p) noexcept :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const D2D1_POINT_2F& p) noexcept :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const D2D1_POINT_2U& p) noexcept :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		~Point() noexcept = default;

		constexpr Point& operator+=(const Point<T>& other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		constexpr Point& operator-=(const Point<T>& other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		constexpr Point& operator*=(const T& factor) noexcept
		{
			x *= factor;
			y *= factor;
			return *this;
		}
		constexpr Point& operator/=(const T& factor) noexcept
		{
			x /= factor;
			y /= factor;
			return *this;
		}
		[[nodiscard]] constexpr Point operator+(const Point& other) const noexcept
		{
			return Point(x + other.x, y + other.y);
		}
		[[nodiscard]] constexpr Point operator-(const Point& other) const noexcept
		{
			return Point(x - other.x, y - other.y);
		}
		[[nodiscard]] constexpr Point operator*(T factor) const noexcept
		{
			return Point(x * factor, y * factor);
		}
		[[nodiscard]] constexpr Point operator/(T factor) const noexcept
		{
			return Point(x / factor, y / factor);
		}
		[[nodiscard]] constexpr Point operator-() const noexcept
		{
			return Point(-x, -y);
		}
		constexpr Point operator+() const noexcept
		{
			return *this;
		}

		[[nodiscard]] constexpr bool operator==(const Point<T>& other) const noexcept = default;

		[[nodiscard]] T Distance(const Point<T>& other) const noexcept
		{
			return Point::Distance(*this, other);
		}
		void Rotate(float angleDegrees, Point<T> point = Point<T>{ }) noexcept
		{
			x -= point.x;
			y -= point.y;

			long double angleRadians = angleDegrees / 180.0 * std::numbers::pi;
			long double x_ = x;
			long double y_ = y;
			x = (T)(x_ * std::cosl(angleRadians) - y_ * std::sinl(angleRadians));
			y = (T)(x_ * std::sinl(angleRadians) + y_ * std::cosl(angleRadians));

			x += point.x;
			y += point.y;
		}

		[[nodiscard]] Point<T> Rotated(float angleDegrees, Point<T> centerPoint = Point<T>{ }) noexcept
		{
			auto point = *this;
			point.Rotate(angleDegrees, centerPoint);
			return point;
		}

		template<typename U>
		explicit(false) operator Point<U>() const noexcept
		{
			return Point<U>{ static_cast<U>(x), static_cast<U>(y) };
		}

		explicit(false) operator POINT() const noexcept
		{
			return POINT{ static_cast<LONG>(x), static_cast<LONG>(y) };
		}
		explicit(false) operator POINTS() const noexcept
		{
			return POINTS{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
		}
		explicit(false) operator D2D1_POINT_2F() const noexcept
		{
			return D2D1_POINT_2F{ static_cast<FLOAT>(x), static_cast<FLOAT>(y) };
		}
		explicit(false) operator D2D1_POINT_2U() const noexcept
		{
			return D2D1_POINT_2U{ static_cast<UINT32>(x), static_cast<UINT32>(y) };
		}
	};

	template<_pt_arithmetic T>
	[[nodiscard]] Point<T> operator*(T factor, const Point<T>& v) noexcept
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<_pt_arithmetic T>
	[[nodiscard]] Point<T> operator/(T factor, const Point<T>& v) noexcept
	{
		return Point<T>(v.x / factor, v.y / factor);
	}

	template<_pt_arithmetic T>
	[[nodiscard]] Point<T> operator*(const Point<T>& v, T factor) noexcept
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<_pt_arithmetic T>
	[[nodiscard]] Point<T> operator/(const Point<T>& v, T factor) noexcept
	{
		return Point<T>(v.x / factor, v.y / factor);
	}


	using PointF = Point<float>;
	using PointI = Point<int>;
	using PointL = Point<long>;
	using PointU = Point<std::uint32_t>;
}
