#include "ui/font/FontCollection.hpp"

#include "helpers/HelperFunctions.hpp"
#include "factories/DWriteFactory.hpp"
#include "ui/font/FontFamily.hpp"
#include "ui/font/FontSet.hpp"


namespace PGUI::UI::Font
{
	FontCollection FontCollection::GetSystemFontCollection()
	{
		auto factory = DWriteFactory::GetFactory();

		ComPtr<IDWriteFontCollection3> fontCollection;

		HRESULT hr = factory->GetSystemFontCollection(false, DWRITE_FONT_FAMILY_MODEL_TYPOGRAPHIC, &fontCollection); HR_T(hr);

		return FontCollection{ fontCollection };
	}
	FontCollection FontCollection::LoadFontFile(std::wstring_view filePath)
	{
		//TODO Implement
		(void)filePath;
		return FontCollection();
	}
	
	FontCollection::FontCollection(ComPtr<IDWriteFontCollection3> _fontCollection) noexcept : 
		ComPtrHolder{ _fontCollection }
	{ }
	
	FontCollection::FontCollection() noexcept :
		ComPtrHolder{ GetSystemFontCollection().GetHeldComPtr() }
	{ }

	FontSet FontCollection::GetFontSet() const noexcept
	{
		return FontSet{ *this };
	}

	FontFamily FontCollection::GetFontFamily(UINT32 index) const noexcept
	{
		ComPtr<IDWriteFontFamily2> fontFamily;

		HRESULT hr = GetHeldComPtr()->GetFontFamily(index, &fontFamily); HR_L(hr);

		return FontFamily{ fontFamily };
	}

	UINT32 FontCollection::GetFontFamilyCount() const noexcept
	{
		return GetHeldComPtr()->GetFontFamilyCount();
	}
}
