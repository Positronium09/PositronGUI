#include "ui/font/FontFamily.hpp"

#include "ui/font/LocalizedFontFamilyNames.hpp"


namespace PGUI::UI::Font
{
	FontFamily::FontFamily(ComPtr<IDWriteFontFamily2> _fontFamily) :
		ComPtrHolder{ _fontFamily }
	{ }

	LocalizedFontFamilyNames FontFamily::GetLocalizedFontFamilyNames() const noexcept
	{
		return LocalizedFontFamilyNames{ *this };
	}
}
