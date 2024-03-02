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

		[[nodiscard]] static T Distance(Point<T> A, Point<T> B)
		{
			return std::hypot(A.x - B.x, A.y - B.y);
		}

		constexpr Point() = default;
		constexpr Point(const T& x_, const T& y_) :
			x{ x_ }, y{ y_ }
		{
		}
		explicit(false) constexpr Point(const POINT& p) :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const POINTS& p) :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const D2D1_POINT_2F& p) :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		explicit(false) constexpr Point(const D2D1_POINT_2U& p) :
			x{ (T)p.x }, y{ (T)p.y }
		{
		}
		~Point() = default;

		constexpr Point& operator+=(const Point<T>& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		constexpr Point& operator-=(const Point<T>& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		constexpr Point& operator*=(const T& factor)
		{
			x *= factor;
			y *= factor;
			return *this;
		}
		constexpr Point& operator/=(const T& factor)
		{
			x /= factor;
			y /= factor;
			return *this;
		}
		constexpr Point operator+(const Point& other) const
		{
			return Point(x + other.x, y + other.y);
		}
		constexpr Point operator-(const Point& other) const
		{
			return Point(x - other.x, y - other.y);
		}
		constexpr Point operator*(T factor) const
		{
			return Point(x * factor, y * factor);
		}
		constexpr Point operator/(T factor) const
		{
			return Point(x / factor, y / factor);
		}
		constexpr Point operator-() const
		{
			return Point(-x, -y);
		}
		constexpr Point operator+() const
		{
			return *this;
		}

		constexpr bool operator==(const Point<T>& other) const = default;

		[[nodiscard]] T Distance(const Point<T>& other) const
		{
			return Point::Distance(*this, other);
		}
		void Rotate(float angleDegrees, Point<T> point = Point<T>{ })
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

		[[nodiscard]] Point<T> Rotated(float angleDegrees, Point<T> centerPoint = Point<T>{ })
		{
			auto point = *this;
			point.Rotate(angleDegrees, centerPoint);
			return point;
		}

		template<typename U>
		explicit operator Point<U>() const
		{
			return Point<U>{ static_cast<U>(x), static_cast<U>(y) };
		}

		explicit(false) operator POINT() const
		{
			return POINT{ static_cast<LONG>(x), static_cast<LONG>(y) };
		}
		explicit(false) operator POINTS() const
		{
			return POINTS{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
		}
		explicit(false) operator D2D1_POINT_2F() const
		{
			return D2D1_POINT_2F{ static_cast<FLOAT>(x), static_cast<FLOAT>(y) };
		}
		explicit(false) operator D2D1_POINT_2U() const
		{
			return D2D1_POINT_2U{ static_cast<UINT32>(x), static_cast<UINT32>(y) };
		}
	};

	template<_pt_arithmetic T>
	Point<T> operator*(T factor, const Point<T>& v)
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<_pt_arithmetic T>
	Point<T> operator/(T factor, const Point<T>& v)
	{
		return Point<T>(v.x / factor, v.y / factor);
	}

	template<_pt_arithmetic T>
	Point<T> operator*(const Point<T>& v, T factor)
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<_pt_arithmetic T>
	Point<T> operator/(const Point<T>& v, T factor)
	{
		return Point<T>(v.x / factor, v.y / factor);
	}


	using PointF = Point<float>;
	using PointI = Point<int>;
	using PointL = Point<long>;
	using PointU = Point<std::uint32_t>;
}
