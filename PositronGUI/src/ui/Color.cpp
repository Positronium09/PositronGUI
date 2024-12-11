#include "ui/Color.hpp"

#include <cmath>
#include <algorithm>


namespace PGUI::UI
{
	#pragma region RGBA

	RGBA::RGBA(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a) noexcept : 
		r(_r), g(_g), b(_b), a(_a)
	{
	}
	RGBA::RGBA(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) noexcept : 
		r(_r / 255.0F), g(_g / 255.0F), b(_b / 255.0F), a(_a / 255.0F)
	{
	}
	RGBA::RGBA(std::uint32_t rgb, FLOAT _a) noexcept : 
		r(((rgb & 0xff << 16) >> 16) / 255.0F), g(((rgb & 0xff << 8) >> 8) / 255.0F), b((rgb & 0xff) / 255.0F), a(_a)
	{
	}

	RGBA::RGBA(HSL hsl) noexcept : 
		a(1.0F)
	{
		auto C = (1 - std::abs(2 * hsl.l - 1)) * hsl.s;
		auto hPrime = hsl.h / 60.0F;

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

		auto m = hsl.l - C / 2.0F;

		r = rPrime + m;
		g = gPrime + m;
		b = bPrime + m;
	}

	RGBA::RGBA(HSV hsv) noexcept : 
		a(1.0F)
	{
		auto C = hsv.v * hsv.s;

		auto hPrime = hsv.h / 60.0F;

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

	RGBA::RGBA(CMYK cmyk) noexcept : 
		r((1 - cmyk.c)* (1 - cmyk.k)),
		g((1 - cmyk.m)* (1 - cmyk.k)),
		b((1 - cmyk.y)* (1 - cmyk.k)),
		a(1.0F)
	{
	}
	RGBA::RGBA(const D2D1_COLOR_F& color) noexcept : 
		r(color.r), g(color.g), b(color.b), a(color.a)
	{
	}

	RGBA::RGBA(const winrt::Windows::UI::Color& color) noexcept : 
		r(color.R / 255.0F), g(color.G / 255.0F), b(color.B / 255.0F), a(color.A / 255.0F)
	{
	}

	RGBA::operator D2D1_COLOR_F() const noexcept
	{
		return D2D1::ColorF(r, g, b, a);
	}

	RGBA::operator winrt::Windows::UI::Color() const noexcept
	{
		winrt::Windows::UI::Color color{ };
		
		color.R = static_cast<BYTE>(r / 255.0F);
		color.G = static_cast<BYTE>(g / 255.0F);
		color.B = static_cast<BYTE>(b / 255.0F);
		color.A = static_cast<BYTE>(a / 255.0F);

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
		r = std::clamp(r + amount, 0.0F, 1.0F);
		g = std::clamp(g + amount, 0.0F, 1.0F);
		b = std::clamp(b + amount, 0.0F, 1.0F);
	}

	void RGBA::Darken(FLOAT amount) noexcept
	{
		r = std::clamp(r - amount, 0.0F, 1.0F);
		g = std::clamp(g - amount, 0.0F, 1.0F);
		b = std::clamp(b - amount, 0.0F, 1.0F);
	}

	auto RGBA::Lightened(FLOAT amount) const noexcept -> RGBA
	{
		auto color = *this;
		color.Lighten(amount);
		return color;
	}

	auto RGBA::Darkened(FLOAT amount) const noexcept -> RGBA
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

	HSL::HSL(const RGBA& rgb) noexcept
	{
		float cMax = std::max({ rgb.r, rgb.g, rgb.b });
		float cMin = std::min({ rgb.r, rgb.g, rgb.b });

		auto delta = cMax - cMin;

		l = (cMax + cMin) / 2.0F;
		s = delta / (1 - std::abs(2 * l - 1));

		if (delta == 0)
		{
			h = 0.0F;
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
		h *= 60.0F;
	}

	HSL::operator RGBA() const noexcept
	{
		return RGBA{ *this };
	}

	#pragma endregion

	#pragma region HSV

	HSV::HSV(FLOAT _h, FLOAT _s, FLOAT _v) noexcept : 
		h(_h), s(_s), v(_v)
	{
	}

	HSV::HSV(const RGBA& rgb) noexcept
	{
		float cMax = std::max({ rgb.r, rgb.g, rgb.b });
		float cMin = std::min({ rgb.r, rgb.g, rgb.b });

		auto delta = cMax - cMin;

		v = cMax;

		if (delta == 0)
		{
			h = 0.0F;
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
		h *= 60.0F;

		if (cMax == 0)
		{
			s = 0.0F;
		}
		else
		{
			s = delta / cMax;
		}
	}

	HSV::operator RGBA() const noexcept
	{
		return RGBA{ *this };
	}

	#pragma endregion

	#pragma region CMYK

	CMYK::CMYK(FLOAT _c, FLOAT _m, FLOAT _y, FLOAT _k) noexcept : 
		c(_c), m(_m), y(_y), k(_k) 
	{
	}

	CMYK::CMYK(const RGBA& rgb) noexcept
	{
		k = std::max({ rgb.r, rgb.g, rgb.b });

		c = (1 - rgb.r - k) / (1 - k);
		m = (1 - rgb.g - k) / (1 - k);
		y = (1 - rgb.b - k) / (1 - k);
	}

	CMYK::operator RGBA() const noexcept
	{
		return RGBA{ *this };
	}

	#pragma endregion
}
