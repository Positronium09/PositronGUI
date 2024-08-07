#include "ui/Color.hpp"

#include <cmath>
#include <algorithm>


namespace PGUI::UI
{
	#pragma region RGB

	RGB::RGB(FLOAT _r, FLOAT _g, FLOAT _b) noexcept :
		r(_r), g(_g), b(_b)
	{
	}
	RGB::RGB(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b) noexcept :
		r(_r / 255.0f), g(_g / 255.0f), b(_b / 255.0f)
	{
	}
	RGB::RGB(std::uint32_t rgb) noexcept :
		r(((rgb & 0xff << 16) >> 16) / 255.0f), g(((rgb & 0xff << 8) >> 8) / 255.0f), b((rgb & 0xff) / 255.0f)
	{
	}

	RGB::RGB(const RGBA& rgba) noexcept :
		r(rgba.r), g(rgba.g), b(rgba.b)
	{
	}
	RGB::RGB(const HSL hsl) noexcept
	{
		auto C = (1 - std::abs(2 * hsl.l - 1)) * hsl.s;
		auto hPrime = hsl.h / 60.0f;

		auto X = C * (1 - std::abs(std::fmodf(hPrime, 2) - 1));

		float rPrime = 0;
		float gPrime = 0;
		float bPrime = 0;

		if (5 <= hPrime)
		{
			rPrime = C;
			bPrime = X;
		}
		else if (4 <= hPrime)
		{
			rPrime = X;
			bPrime = C;
		}
		else if (3 <= hPrime)
		{
			gPrime = X;
			bPrime = C;
		}
		else if (2 <= hPrime)
		{
			gPrime = C;
			bPrime = X;
		}
		else if (1 <= hPrime)
		{
			rPrime = X;
			gPrime = C;
		}
		else
		{
			rPrime = C;
			gPrime = X;
		}

		auto m = hsl.l - C / 2.0f;

		r = rPrime + m;
		g = gPrime + m;
		b = bPrime + m;
	}
	RGB::RGB(const HSV hsv) noexcept
	{
		auto C = hsv.v * hsv.s;

		auto hPrime = hsv.h / 60.0f;

		auto X = C * (1 - std::abs(std::fmodf(hPrime, 2) - 1));

		float rPrime = 0;
		float gPrime = 0;
		float bPrime = 0;

		if (5 <= hPrime)
		{
			rPrime = C;
			bPrime = X;
		}
		else if (4 <= hPrime)
		{
			rPrime = X;
			bPrime = C;
		}
		else if (3 <= hPrime)
		{
			gPrime = X;
			bPrime = C;
		}
		else if (2 <= hPrime)
		{
			gPrime = C;
			bPrime = X;
		}
		else if (1 <= hPrime)
		{
			rPrime = X;
			gPrime = C;
		}
		else
		{
			rPrime = C;
			gPrime = X;
		}

		auto m = hsv.v - C;

		r = rPrime + m;
		g = gPrime + m;
		b = bPrime + m;

	}
	RGB::RGB(const CMYK cmyk) noexcept :
		r((1 - cmyk.c)* (1 - cmyk.k)), 
		g((1 - cmyk.m)* (1 - cmyk.k)),
		b((1 - cmyk.y) * (1 - cmyk.k))
	{
	}

	RGB::RGB(const D2D1_COLOR_F& color) noexcept :
		r(color.r), g(color.g), b(color.b)
	{
	}

	RGB::operator D2D1_COLOR_F() const noexcept
	{
		return D2D1::ColorF(r, g, b);
	}

	void RGB::Lighten(FLOAT amount) noexcept
	{
		r = std::clamp(r + amount, 0.0f, 1.0f);
		g = std::clamp(g + amount, 0.0f, 1.0f);
		b = std::clamp(b + amount, 0.0f, 1.0f);
	}

	void RGB::Darken(FLOAT amount) noexcept
	{
		r = std::clamp(r - amount, 0.0f, 1.0f);
		g = std::clamp(g - amount, 0.0f, 1.0f);
		b = std::clamp(b - amount, 0.0f, 1.0f);
	}

	RGB RGB::Lightened(FLOAT amount) const noexcept
	{
		auto color = *this;
		color.Lighten(amount);
		return color;
	}

	RGB RGB::Darkened(FLOAT amount) const noexcept
	{
		auto color = *this;
		color.Darken(amount);
		return color;
	}

	#pragma endregion

	#pragma region RGBA

	RGBA::RGBA(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a) noexcept : 
		r(_r), g(_g), b(_b), a(_a)
	{
	}
	RGBA::RGBA(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) noexcept : 
		r(_r / 255.0f), g(_g / 255.0f), b(_b / 255.0f), a(_a / 255.0f)
	{
	}
	RGBA::RGBA(std::uint32_t rgb, FLOAT _a) noexcept : 
		r(((rgb & 0xff << 16) >> 16) / 255.0f), g(((rgb & 0xff << 8) >> 8) / 255.0f), b((rgb & 0xff) / 255.0f), a(_a)
	{
	}

	RGBA::RGBA(const RGB& rgb) noexcept :
		r(rgb.r), g(rgb.g), b(rgb.b), a(1.0f)
	{
	}
	RGBA::RGBA(const D2D1_COLOR_F& color) noexcept : 
		r(color.r), g(color.g), b(color.b), a(color.a)
	{
	}

	RGBA::RGBA(const winrt::Windows::UI::Color& color) noexcept : 
		r(color.R / 255.0f), g(color.G / 255.0f), b(color.B / 255.0f), a(color.A / 255.0f)
	{
	}

	RGBA::operator RGB() noexcept
	{
		return RGB{ *this };
	}
	RGBA::operator D2D1_COLOR_F() const noexcept
	{
		return D2D1::ColorF(r, g, b, a);
	}

	RGBA::operator winrt::Windows::UI::Color() const noexcept
	{
		winrt::Windows::UI::Color color{ };
		
		color.R = static_cast<BYTE>(r / 255.0f);
		color.G = static_cast<BYTE>(g / 255.0f);
		color.B = static_cast<BYTE>(b / 255.0f);
		color.A = static_cast<BYTE>(a / 255.0f);

		return color;
	}

	RGBA::operator COLORREF() const noexcept
	{
		auto R = static_cast<DWORD>(r * 255);
		auto G = static_cast<DWORD>(g * 255);
		auto B = static_cast<DWORD>(b * 255);

		return R | (G << 8) | (B << 16);
	}

	void RGBA::Lighten(FLOAT amount) noexcept
	{
		r = std::clamp(r + amount, 0.0f, 1.0f);
		g = std::clamp(g + amount, 0.0f, 1.0f);
		b = std::clamp(b + amount, 0.0f, 1.0f);
	}

	void RGBA::Darken(FLOAT amount) noexcept
	{
		r = std::clamp(r - amount, 0.0f, 1.0f);
		g = std::clamp(g - amount, 0.0f, 1.0f);
		b = std::clamp(b - amount, 0.0f, 1.0f);
	}

	RGBA RGBA::Lightened(FLOAT amount) const noexcept
	{
		auto color = *this;
		color.Lighten(amount);
		return color;
	}

	RGBA RGBA::Darkened(FLOAT amount) const noexcept
	{
		auto color = *this;
		color.Darken(amount);
		return color;
	}

	#pragma endregion

	#pragma region HSL

	HSL::HSL(FLOAT _h, FLOAT _s, FLOAT _l) noexcept : 
		h(_h), s(_s), l(_l)
	{
	}

	HSL::HSL(const RGB& rgb) noexcept
	{
		float cMax = std::max({ rgb.r, rgb.g, rgb.b });
		float cMin = std::min({ rgb.r, rgb.g, rgb.b });

		auto delta = cMax - cMin;

		l = (cMax + cMin) / 2.0f;
		s = delta / (1 - std::abs(2 * l - 1));

		if (delta == 0)
		{
			h = 0.0f;
		}
		else if (cMax == rgb.r)
		{
			h = std::fmodf(rgb.g - rgb.b / delta, 6);
		}
		else if (cMax == rgb.g)
		{
			h = (rgb.b - rgb.r) / delta + 2;
		}
		else
		{
			h = (rgb.r - rgb.g) / delta + 4;
		}
		h *= 60.0f;
	}

	HSL::operator RGB() const noexcept
	{
		return RGB{ *this };
	}

	#pragma endregion

	#pragma region HSV

	HSV::HSV(FLOAT _h, FLOAT _s, FLOAT _v) noexcept : 
		h(_h), s(_s), v(_v)
	{
	}

	HSV::HSV(const RGB& rgb) noexcept
	{
		float cMax = std::max({ rgb.r, rgb.g, rgb.b });
		float cMin = std::min({ rgb.r, rgb.g, rgb.b });

		auto delta = cMax - cMin;

		v = cMax;

		if (delta == 0)
		{
			h = 0.0f;
		}
		else if (cMax == rgb.r)
		{
			h = std::fmodf(rgb.g - rgb.b / delta, 6);
		}
		else if (cMax == rgb.g)
		{
			h = (rgb.b - rgb.r) / delta + 2;
		}
		else
		{
			h = (rgb.r - rgb.g) / delta + 4;
		}
		h *= 60.0f;

		if (cMax == 0)
		{
			s = 0.0f;
		}
		else
		{
			s = delta / cMax;
		}
	}

	HSV::operator RGB() const noexcept
	{
		return RGB{ *this };
	}

	#pragma endregion

	#pragma region CMYK

	CMYK::CMYK(FLOAT _c, FLOAT _m, FLOAT _y, FLOAT _k) noexcept : 
		c(_c), m(_m), y(_y), k(_k) 
	{
	}

	CMYK::CMYK(const RGB& rgb) noexcept
	{
		k = std::max({ rgb.r, rgb.g, rgb.b });

		c = (1 - rgb.r - k) / (1 - k);
		m = (1 - rgb.g - k) / (1 - k);
		y = (1 - rgb.b - k) / (1 - k);
	}

	CMYK::operator RGB() const noexcept
	{
		return RGB{ *this };
	}

	#pragma endregion
}
