#include "ui/font/FontSet.hpp"

#include "helpers/HelperFunctions.hpp"
#include "core/Exceptions.hpp"
#include "ui/font/FontCollection.hpp"


namespace PGUI::UI::Font
{
	FontSet::FontSet(const FontCollection& fontCollection)
	{
		ComPtr<IDWriteFontSet1> fs;

		HRESULT hr = fontCollection->GetFontSet(&fs); HR_T(hr);
		hr = fs.As(GetHeldComPtrAddress()); HR_T(hr);
	}

	auto FontSet::ConvertWeightStretchStyleToAxisValues(
		FontWeight fontWeight, FontStretch fontStretch, FontStyle fontStyle, float fontSize) const noexcept -> std::array<DWRITE_FONT_AXIS_VALUE, 5>
	{
		std::array<DWRITE_FONT_AXIS_VALUE, 5> axisValues{ };
		GetHeldComPtr()->ConvertWeightStretchStyleToFontAxisValues(
			axisValues.data(), 0, fontWeight, fontStretch, fontStyle, fontSize, axisValues.data());

		return axisValues;
	}
}
