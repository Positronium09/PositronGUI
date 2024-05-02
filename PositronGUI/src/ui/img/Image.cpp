#include "ui/img/Image.hpp"

#include "ui/img/Frame.hpp"
#include "ui/img/ImageMetadataReader.hpp"
#include "core/Logger.hpp"
#include "factories/WICFactory.hpp"


namespace PGUI::UI::Img
{
	Image::Image(std::wstring_view filePath, bool readOnly, std::optional<GUID> vendorGUID) noexcept
	{
		auto wicFactory = PGUI::WICFactory::GetFactory();

		const GUID* vendor = nullptr;
		if (vendorGUID.has_value())
		{
			vendor = &vendorGUID.value();
		}

		DWORD access = GENERIC_READ | (!readOnly ? GENERIC_WRITE : 0);

		HRESULT hr = wicFactory->CreateDecoderFromFilename(
			filePath.data(), vendor, access, WICDecodeMetadataCacheOnDemand, GetHeldComPtrAddress()); HR_L(hr);
	}

	Image::Image(ULONG_PTR fileHandle, std::optional<GUID> vendorGUID) noexcept
	{
		auto wicFactory = PGUI::WICFactory::GetFactory();

		const GUID* vendor = nullptr;
		if (vendorGUID.has_value())
		{
			vendor = &vendorGUID.value();
		}

		HRESULT hr = wicFactory->CreateDecoderFromFileHandle(
			fileHandle, vendor, WICDecodeMetadataCacheOnDemand, GetHeldComPtrAddress()); HR_L(hr);
	}

	/*
	Image::Image(ComPtr<IStream> stream, std::optional<GUID> vendorGUID) noexcept
	{
		auto wicFactory = PGUI::WICFactory::GetFactory();

		const GUID* vendor = nullptr;
		if (vendorGUID.has_value())
		{
			vendor = &vendorGUID.value();
		}

		HRESULT hr = wicFactory->CreateDecoderFromStream(
			stream.Get(), vendor, WICDecodeMetadataCacheOnDemand, GetHeldComPtrAddress()); HR_L(hr);
	}
	*/

	Image::Image() noexcept : 
		ComPtrHolder{ nullptr }
	{
	}

	Frame Image::GetFrame(UINT frameIndex) const noexcept
	{
		return Frame{ *this, frameIndex };
	}

	std::vector<Frame> Image::GetFrames() const noexcept
	{
		auto frameCount = GetFrameCount();
		std::vector<Frame> frames(frameCount);

		for (UINT i = 0; i < frameCount; i++)
		{
			frames[i] = GetFrame(i);
		}

		return frames;
	}

	UINT Image::GetFrameCount() const noexcept
	{
		UINT count = 0;

		HRESULT hr = GetHeldComPtr()->GetFrameCount(&count); HR_L(hr);

		return count;
	}

	ImageMetadataReader Image::GetMetadata() const noexcept
	{
		return ImageMetadataReader{ *this };
	}

	ComPtr<IWICBitmapSource> Image::GetThumbnail() const noexcept
	{
		ComPtr<IWICBitmapSource> thumbnail;

		HRESULT hr = GetHeldComPtr()->GetThumbnail(thumbnail.GetAddressOf());  HR_L(hr);

		return thumbnail;
	}
}
