#pragma once

#include "helpers/ComPtrHolder.hpp"

#include <dwrite_3.h>


namespace PGUI::UI::Font
{
	class LocalizedFontFamilyNames;

	class FontFamily : public ComPtrHolder<IDWriteFontFamily2>
	{
		public:
		FontFamily() = default;
		explicit FontFamily(ComPtr<IDWriteFontFamily2> fontFamily);

		[[nodiscard]] LocalizedFontFamilyNames GetLocalizedFontFamilyNames() const noexcept;
	};
}
