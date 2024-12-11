#include <utility>

#include "ui/font/FontCollection.hpp"

#include "helpers/HelperFunctions.hpp"
#include "factories/DWriteFactory.hpp"
#include "ui/font/FontFamily.hpp"
#include "ui/font/FontSet.hpp"


namespace PGUI::UI::Font
{
	auto FontCollection::GetSystemFontCollection() -> FontCollection
	{
		auto factory = DWriteFactory::GetFactory();

		ComPtr<IDWriteFontCollection3> fontCollection;

		HRESULT hr = factory->GetSystemFontCollection(false, DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC, &fontCollection); HR_T(hr);

		return FontCollection{ fontCollection };
	}
	auto FontCollection::LoadFontFile(std::wstring_view filePath) -> FontCollection
	{
		//TODO Implement
		(void)filePath;
		return {};
	}
	
	FontCollection::FontCollection(ComPtr<IDWriteFontCollection3> _fontCollection) noexcept : 
		ComPtrHolder{ std::move(_fontCollection) }
	{ }
	
	FontCollection::FontCollection() noexcept :
		ComPtrHolder{ GetSystemFontCollection().GetHeldComPtr() }
	{ }

	auto FontCollection::GetFontSet() const noexcept -> FontSet
	{
		return FontSet{ *this };
	}

	auto FontCollection::GetFontFamily(UINT32 index) const noexcept -> FontFamily
	{
		ComPtr<IDWriteFontFamily2> fontFamily;

		HRESULT hr = GetHeldComPtr()->GetFontFamily(index, &fontFamily); HR_L(hr);

		return FontFamily{ fontFamily };
	}

	auto FontCollection::GetFontFamilyCount() const noexcept -> UINT32
	{
		return GetHeldComPtr()->GetFontFamilyCount();
	}
}
