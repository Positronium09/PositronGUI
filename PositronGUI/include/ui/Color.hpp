#pragma once

#include <cstdint>
#include <d2d1.h>
#include <Windows.h>


#undef RGB
#undef CMYK

namespace PGUI::UI
{
	class RGBA;
	class HSL;
	class HSV;
	class CMYK;

	class RGB
	{
		public:
		RGB() noexcept = default;
		RGB(FLOAT r, FLOAT g, FLOAT b) noexcept;
		RGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept;
		explicit RGB(std::uint32_t rgb) noexcept;

		explicit(false) RGB(const RGBA& rgba) noexcept;
		explicit(false) RGB(const HSL hsl) noexcept;
		explicit(false) RGB(const HSV hsv) noexcept;
		explicit(false) RGB(const CMYK cmyk) noexcept;

		explicit(false) RGB(const D2D1_COLOR_F& color) noexcept;

		explicit(false) operator D2D1_COLOR_F() const noexcept;

		constexpr [[nodiscard]] bool operator==(const RGB& other) const noexcept = default;

		constexpr void Lighten(FLOAT amount) noexcept;
		constexpr void Darken(FLOAT amount) noexcept;

		FLOAT r = 0.0f;
		FLOAT g = 0.0f;
		FLOAT b = 0.0f;
	};

	class RGBA
	{
		public:
		RGBA() noexcept = default;
		RGBA(FLOAT r, FLOAT g, FLOAT b, FLOAT a=1.0f) noexcept;
		RGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a=255) noexcept;
		RGBA(std::uint32_t rgb, FLOAT a=1.0f) noexcept;

		explicit(false) RGBA(const RGB& rgb) noexcept;

		explicit(false) RGBA(const D2D1_COLOR_F& color) noexcept;

		explicit(false) operator RGB() noexcept;
		explicit(false) operator D2D1_COLOR_F() const noexcept;

		constexpr [[nodiscard]] bool operator==(const RGBA& other) const noexcept = default;

		constexpr void Lighten(FLOAT amount) noexcept;
		constexpr void Darken(FLOAT amount) noexcept;

		FLOAT r = 0.0f;
		FLOAT g = 0.0f;
		FLOAT b = 0.0f;
		FLOAT a = 0.0f;
	};

	class HSL
	{
		public:
		HSL() noexcept = default;
		HSL(FLOAT h, FLOAT s, FLOAT l) noexcept;

		explicit(false) HSL(const RGB& rgb) noexcept;

		explicit(false) operator RGB() const noexcept;

		constexpr [[nodiscard]] bool operator==(const HSL& other) const noexcept = default;

		FLOAT h = 0.0f;
		FLOAT s = 0.0f;
		FLOAT l = 0.0f;
	};

	class HSV
	{
		public:
		HSV() noexcept = default;
		HSV(FLOAT h, FLOAT s, FLOAT v) noexcept;

		explicit(false) HSV(const RGB& rgb) noexcept;

		explicit(false) operator RGB() const noexcept;

		constexpr [[nodiscard]] bool operator==(const HSV& other) const noexcept = default;

		FLOAT h = 0.0f;
		FLOAT s = 0.0f;
		FLOAT v = 0.0f;
	};

	class CMYK
	{
		public:
		CMYK() noexcept = default;
		CMYK(FLOAT c, FLOAT m, FLOAT y, FLOAT k) noexcept;

		explicit(false) CMYK(const RGB& rgb) noexcept;

		explicit(false) operator RGB() const noexcept;

		constexpr [[nodiscard]] bool operator==(const CMYK& other) const noexcept = default;

		FLOAT c = 0.0f;
		FLOAT m = 0.0f;
		FLOAT y = 0.0f;
		FLOAT k = 0.0f;
	};
}
