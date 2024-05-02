#include "ui/img/ImageMetadataReader.hpp"

#include "core/Logger.hpp"
#include "core/Exceptions.hpp"


namespace PGUI::UI::Img
{
	ImageMetadataReader::ImageMetadataReader(Frame frame) noexcept
	{
		HRESULT hr = frame->GetMetadataQueryReader(GetHeldComPtrAddress()); HR_T(hr);
	}
	ImageMetadataReader::ImageMetadataReader(Image img) noexcept
	{
		HRESULT hr = img->GetMetadataQueryReader(GetHeldComPtrAddress()); HR_T(hr);
	}
	ImageMetadataReader::ImageMetadataReader(ComPtr<IWICMetadataQueryReader> reader) noexcept : 
		ComPtrHolder{ reader }

	{
	}

	PropVariant ImageMetadataReader::GetMetadata(std::wstring_view name) const noexcept
	{
		PropVariant variant;

		HRESULT hr = GetHeldComPtr()->GetMetadataByName(name.data(), &variant); HR_L(hr);

		return variant;
	}
	std::wstring ImageMetadataReader::Location() const noexcept
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
}
