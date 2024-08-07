#include "ui/bmp/MetadataReader.hpp"

#include "helpers/HelperFunctions.hpp"
#include "core/Exceptions.hpp"


namespace PGUI::UI::Bmp
{
	MetadataReader::MetadataReader(Frame frame) noexcept
	{
		auto frameDecode = (IWICBitmapFrameDecode*)frame;
		HRESULT hr = frameDecode->GetMetadataQueryReader(GetHeldPtrAddress()); HR_T(hr);
	}
	MetadataReader::MetadataReader(BitmapDecoder img) noexcept
	{
		HRESULT hr = img->GetMetadataQueryReader(GetHeldPtrAddress()); HR_T(hr);
	}
	MetadataReader::MetadataReader(ComPtr<IWICMetadataQueryReader> reader) noexcept : 
		ComPtrHolder{ reader }

	{
	}

	PropVariant MetadataReader::GetMetadata(std::wstring_view name) const noexcept
	{
		PropVariant variant;

		HRESULT hr = GetHeldComPtr()->GetMetadataByName(name.data(), &variant); HR_L(hr);

		return variant;
	}
	std::wstring MetadataReader::Location() const noexcept
	{
		auto ptr = GetHeldComPtr();

		UINT required = 0;
		HRESULT hr = ptr->GetLocation(NULL, nullptr, &required); HR_L(hr);

		if (!required)
		{
			return L"";
		}

		std::wstring location(required - 1ULL, L'\0');
		hr = ptr->GetLocation(required, location.data(), &required); HR_L(hr);

		return location;
	}
	PropVariant MetadataReader::operator[](std::wstring_view name) const noexcept
	{
		return GetMetadata(name);
	}
}
