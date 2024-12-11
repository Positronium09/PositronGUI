#include <utility>

#include "ui/font/FontFamily.hpp"

#include "ui/font/LocalizedFontFamilyNames.hpp"


namespace PGUI::UI::Font
{
	FontFamily::FontFamily(ComPtr<IDWriteFontFamily2> _fontFamily) :
		ComPtrHolder{ std::move(_fontFamily) }
	{ }

	auto FontFamily::GetLocalizedFontFamilyNames() const noexcept -> LocalizedFontFamilyNames
	{
		return LocalizedFontFamilyNames{ *this };
	}
}
