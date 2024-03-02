#pragma once

#include <cstdint>
#include <d2d1.h>
#include <Windows.h>


#undef RGB
#undef CMYK

namespace PGUI
{
	class RGBA;
	class HSL;
	class HSV;
	class CMYK;

	class RGB
	{
		public:
		RGB();
		RGB(FLOAT r, FLOAT g, FLOAT b);
		RGB(std::uint8_t r, std::uint8_t g, std::uint8_t b);
		explicit RGB(std::uint32_t rgb);

		explicit(false) RGB(const RGBA& rgba);
		explicit(false) RGB(const HSL hsl);
		explicit(false) RGB(const HSV hsv);
		explicit(false) RGB(const CMYK cmyk);

		explicit(false) RGB(const D2D1_COLOR_F& color);

		explicit(false) operator D2D1_COLOR_F() const;

		bool operator==(const RGB& other) const = default;

		void Lighten(FLOAT amount);
		void Darken(FLOAT amount);

		FLOAT r;
		FLOAT g;
		FLOAT b;
	};

	class RGBA
	{
		public:
		RGBA();
		RGBA(FLOAT r, FLOAT g, FLOAT b, FLOAT a=1.0f);
		RGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a=255);
		RGBA(std::uint32_t rgb, FLOAT a=1.0f);

		explicit(false) RGBA(const RGB& rgb);

		explicit(false) RGBA(const D2D1_COLOR_F& color);

		explicit(false) operator RGB();
		explicit(false) operator D2D1_COLOR_F() const;

		bool operator==(const RGBA& other) const = default;

		void Lighten(FLOAT amount);
		void Darken(FLOAT amount);

		FLOAT r;
		FLOAT g;
		FLOAT b;
		FLOAT a;
	};

	class HSL
	{
		public:
		HSL();
		HSL(FLOAT h, FLOAT s, FLOAT l);

		explicit(false) HSL(const RGB& rgb);

		explicit(false) operator RGB() const;

		bool operator==(const HSL& other) const = default;

		FLOAT h;
		FLOAT s;
		FLOAT l;
	};

	class HSV
	{
		public:
		HSV();
		HSV(FLOAT h, FLOAT s, FLOAT v);

		explicit(false) HSV(const RGB& rgb);

		explicit(false) operator RGB() const;

		bool operator==(const HSV& other) const = default;

		FLOAT h;
		FLOAT s;
		FLOAT v;
	};

	class CMYK
	{
		public:
		CMYK();
		CMYK(FLOAT c, FLOAT m, FLOAT y, FLOAT k);

		explicit(false) CMYK(const RGB& rgb);

		explicit(false) operator RGB() const;

		bool operator==(const CMYK& other) const = default;

		FLOAT c;
		FLOAT m;
		FLOAT y;
		FLOAT k;
	};
}
