#pragma once

#include <cmath>
#include <cstdint>
#include <numbers>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>


namespace PGUI
{
	template<typename T> requires std::is_arithmetic_v<T>
	struct Point
	{
		T x = static_cast<T>(0);
		T y = static_cast<T>(0);

		[[nodiscard]] static auto Distance(Point<T> A, Point<T> B) noexcept
		{
			return static_cast<T>(std::sqrt(static_cast<double>(DistanceSqr(A, B))));
		}
		[[nodiscard]] static constexpr auto DistanceSqr(Point<T> A, Point<T> B) noexcept
		{
			return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
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

		constexpr auto& operator+=(const Point& other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		constexpr auto& operator-=(const Point& other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		constexpr auto& operator*=(const T& factor) noexcept
		{
			x *= factor;
			y *= factor;
			return *this;
		}
		constexpr auto& operator/=(const T& factor) noexcept
		{
			x /= factor;
			y /= factor;
			return *this;
		}
		[[nodiscard]] constexpr auto operator+(const Point& other) const noexcept
		{
			return Point{ x + other.x, y + other.y };
		}
		[[nodiscard]] constexpr auto operator-(const Point& other) const noexcept
		{
			return Point(x - other.x, y - other.y);
		}
		[[nodiscard]] constexpr auto operator*(T factor) const noexcept
		{
			return Point(x * factor, y * factor);
		}
		[[nodiscard]] constexpr auto operator/(T factor) const noexcept
		{
			return Point(x / factor, y / factor);
		}
		[[nodiscard]] constexpr auto operator-() const noexcept
		{
			return Point{ -x, -y };
		}
		constexpr auto operator+() const noexcept -> Point
		{
			return *this;
		}

		[[nodiscard]] constexpr auto operator==(const Point& other) const noexcept -> bool = default;

		[[nodiscard]] auto Distance(const Point<T>& other) const noexcept
		{
			return Point::Distance(*this, other);
		}
		[[nodiscard]] constexpr auto DistanceSqr(const Point& other) const noexcept
		{
			return Point::DistanceSqr(*this, other);
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

		[[nodiscard]] auto Rotated(float angleDegrees, Point<T> centerPoint = Point<T>{ }) noexcept
		{
			auto point = *this;
			point.Rotate(angleDegrees, centerPoint);
			return point;
		}

		template<typename U> requires std::is_arithmetic_v<U>
		explicit(false) constexpr operator Point<U>() const noexcept
		{
			return Point<U>{ static_cast<U>(x), static_cast<U>(y) };
		}

		explicit(false) constexpr operator POINT() const noexcept
		{
			return POINT{ static_cast<LONG>(x), static_cast<LONG>(y) };
		}
		explicit(false) constexpr operator POINTS() const noexcept
		{
			return POINTS{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
		}
		explicit(false) constexpr operator D2D1_POINT_2F() const noexcept
		{
			return D2D1_POINT_2F{ static_cast<FLOAT>(x), static_cast<FLOAT>(y) };
		}
		explicit(false) constexpr operator D2D1_POINT_2U() const noexcept
		{
			return D2D1_POINT_2U{ static_cast<UINT32>(x), static_cast<UINT32>(y) };
		}
	};

	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr auto operator*(T factor, const Point<T>& v) noexcept
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr auto operator/(T factor, const Point<T>& v) noexcept
	{
		return Point<T>(v.x / factor, v.y / factor);
	}

	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr auto operator*(const Point<T>& v, T factor) noexcept
	{
		return Point<T>(v.x * factor, v.y * factor);
	}

	template<typename T> requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr auto operator/(const Point<T>& v, T factor) noexcept
	{
		return Point<T>(v.x / factor, v.y / factor);
	}


	using PointF = Point<float>;
	using PointI = Point<int>;
	using PointL = Point<long>;
	using PointU = Point<std::uint32_t>;
}
