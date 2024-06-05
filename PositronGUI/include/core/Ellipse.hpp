#pragma once

#include "Point.hpp"


namespace PGUI
{
	class Ellipse
	{
		public:
		PointF center;
		float xRadius = 0.0f;
		float yRadius = 0.0f;

		constexpr Ellipse() noexcept = default;
		constexpr Ellipse(PointF _center, float radius) noexcept :
			center(_center), xRadius(radius), yRadius(radius)
		{
		}
		constexpr Ellipse(PointF _center, float _xRadius, float _yRadius) noexcept : 
			center(_center), xRadius(_xRadius), yRadius(_yRadius)
		{
		}
		explicit(false) constexpr Ellipse(D2D1_ELLIPSE ellipse) noexcept : 
			center(ellipse.point), xRadius(ellipse.radiusX), yRadius(ellipse.radiusY)
		{
		}
		~Ellipse() noexcept = default;

		[[nodiscard]] constexpr bool operator==(const Ellipse& other) const noexcept = default;

		explicit(false) operator D2D1_ELLIPSE() const noexcept
		{
			return D2D1_ELLIPSE{ center, xRadius, yRadius };
		}
	};
}
