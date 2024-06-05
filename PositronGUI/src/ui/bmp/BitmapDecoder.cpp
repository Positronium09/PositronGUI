#include "ui/bmp/BitmapDecoder.hpp"

#include "ui/bmp/Frame.hpp"
#include "ui/bmp/MetadataReader.hpp"
#include "core/Logger.hpp"
#include "factories/WICFactory.hpp"


namespace PGUI::UI::Bmp
{
	BitmapDecoder::BitmapDecoder(std::wstring_view filePath, bool readOnly, std::optional<GUID> vendorGUID) noexcept
	{
		auto wicFactory = PGUI::WICFactory::GetFactory();

		const GUID* vendor = nullptr;
		if (vendorGUID.has_value())
		{
			vendor = &vendorGUID.value();
		}

		DWORD access = GENERIC_READ | (!readOnly ? GENERIC_WRITE : 0);

		HRESULT hr = wicFactory->CreateDecoderFromFilename(
			filePath.data(), vendor, access, WICDecodeMetadataCacheOnDemand, GetHeldPtrAddress()); HR_L(hr);
	}

	BitmapDecoder::BitmapDecoder(ULONG_PTR fileHandle, std::optional<GUID> vendorGUID) noexcept
	{
		auto wicFactory = PGUI::WICFactory::GetFactory();

		const GUID* vendor = nullptr;
		if (vendorGUID.has_value())
		{
			vendor = &vendorGUID.value();
		}

		HRESULT hr = wicFactory->CreateDecoderFromFileHandle(
			fileHandle, vendor, WICDecodeMetadataCacheOnDemand, GetHeldPtrAddress()); HR_L(hr);
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

	BitmapDecoder::BitmapDecoder() noexcept : 
		ComPtrHolder{ nullptr }
	{
	}

	Frame BitmapDecoder::GetFrame(UINT frameIndex) const noexcept
	{
		return Frame{ *this, frameIndex };
	}

	std::vector<Frame> BitmapDecoder::GetFrames() const noexcept
	{
		auto frameCount = GetFrameCount();
		std::vector<Frame> frames(frameCount);

		for (UINT i = 0; i < frameCount; i++)
		{
			frames[i] = GetFrame(i);
		}

		return frames;
	}

	UINT BitmapDecoder::GetFrameCount() const noexcept
	{
		UINT count = 0;

		HRESULT hr = GetHeldComPtr()->GetFrameCount(&count); HR_L(hr);

		return count;
	}

	MetadataReader BitmapDecoder::GetMetadata() const noexcept
	{
		return MetadataReader{ *this };
	}

	ComPtr<IWICBitmapSource> BitmapDecoder::GetThumbnail() const noexcept
	{
		ComPtr<IWICBitmapSource> thumbnail;

		HRESULT hr = GetHeldComPtr()->GetThumbnail(thumbnail.GetAddressOf());  HR_L(hr);

		return thumbnail;
	}
}
