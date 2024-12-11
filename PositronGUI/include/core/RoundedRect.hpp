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
		float xRadius = 0.0F;
		float yRadius = 0.0F;

		using Rect::Rect;
		constexpr RoundedRect(float left_, float top_, float right_, float bottom_, float xRadius_ = 0.0F, float yRadius_ = 0.0F) noexcept :
			Rect{ left_, top_, right_, bottom_ }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		template<typename T> requires std::is_arithmetic_v<T>
		explicit(false) constexpr RoundedRect(const Rect<T>& rc, float xRadius_ = 0.0F, float yRadius_ = 0.0F) noexcept :
			Rect<float>{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const RECT& rc, float xRadius_ = 0.0F, float yRadius_ = 0.0F) noexcept :
			Rect<float>{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_RECT_F& rc, float xRadius_ = 0.0F, float yRadius_ = 0.0F) noexcept :
			Rect<float>{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_RECT_U& rc, float xRadius_ = 0.0F, float yRadius_ = 0.0F) noexcept :
			Rect<float>{ rc }, xRadius(xRadius_), yRadius(yRadius_)
		{
		}
		explicit(false) constexpr RoundedRect(const D2D1_ROUNDED_RECT& rrc) noexcept :
			Rect<float>{ rrc.rect }, xRadius(rrc.radiusX), yRadius(rrc.radiusY)
		{
		}
		
		[[nodiscard]] constexpr auto operator==(const RoundedRect& other) const noexcept -> bool = default;

		explicit(false) operator D2D1_ROUNDED_RECT() const noexcept
		{
			return D2D1_ROUNDED_RECT{ *this, xRadius, yRadius };
		}
	};
}
