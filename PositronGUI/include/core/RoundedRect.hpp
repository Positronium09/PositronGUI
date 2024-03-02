#pragma once

#include "Point.hpp"
#include "Size.hpp"
#include "Rect.hpp"

#include <cstdint>
#include <type_traits>
#include <d2d1_1.h>
#include <Windows.h>


namespace PGUI
{
	struct RoundedRect : public Rect<float>
	{
		public:
		float xRadius = 0.0f;
		float yRadius = 0.0f;

		using Rect::Rect;
		constexpr RoundedRect(float left_, float top_, float right_, float bottom_, float xRadius_ = 0.0f, float yRadius_ = 0.0f) :
			Rect{ left_, top_, right_, bottom_ }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		template <_rc_arithmetic T>
		explicit(false) constexpr RoundedRect(const Rect<T>& rc, float xRadius_ = 0.0f, float yRadius_ = 0.0f) :
			Rect{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const RECT& rc, float xRadius_ = 0.0f, float yRadius_ = 0.0f) :
			Rect{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_RECT_F& rc, float xRadius_ = 0.0f, float yRadius_ = 0.0f) :
			Rect{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_RECT_U& rc, float xRadius_ = 0.0f, float yRadius_ = 0.0f) :
			Rect{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_ROUNDED_RECT& rrc) :
			Rect{ rrc.rect }, xRadius(rrc.radiusX), yRadius(rrc.radiusY)
		{
		}
		~RoundedRect() = default;

		constexpr bool operator==(const RoundedRect& other) const = default;

		explicit(false) operator D2D1_ROUNDED_RECT() const
		{
			return D2D1_ROUNDED_RECT{ *this, xRadius, yRadius };
		}
	};
}
