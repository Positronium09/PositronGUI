#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "FontEnums.hpp"

#include <array>
#include <dwrite_3.h>


namespace PGUI::UI::Font
{
	class FontCollection;

	class FontSet : public ComPtrHolder<IDWriteFontSet4>
	{
		public:
		explicit FontSet(const FontCollection& fontCollection);

		[[nodiscard]] std::array<DWRITE_FONT_AXIS_VALUE, 5> ConvertWeightStretchStyleToAxisValues(
			FontWeight fontWeight, FontStretch fontStretch, FontStyle fontStyle, float fontSize) const noexcept;
	};
}
