#include "ui/bmp/BitmapDecoder.hpp"

#include "ui/bmp/Frame.hpp"
#include "ui/bmp/MetadataReader.hpp"
#include "helpers/HelperFunctions.hpp"
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

	auto BitmapDecoder::GetFrame(UINT frameIndex) const noexcept -> Frame
	{
		return Frame{ *this, frameIndex };
	}

	auto BitmapDecoder::GetFrames() const noexcept -> std::vector<Frame>
	{
		auto frameCount = GetFrameCount();
		std::vector<Frame> frames(frameCount);

		for (UINT i = 0; i < frameCount; i++)
		{
			frames[i] = GetFrame(i);
		}

		return frames;
	}

	auto BitmapDecoder::GetFrameCount() const noexcept -> UINT
	{
		UINT count = 0;

		HRESULT hr = GetHeldComPtr()->GetFrameCount(&count); HR_L(hr);

		return count;
	}

	auto BitmapDecoder::GetMetadata() const noexcept -> MetadataReader
	{
		return MetadataReader{ *this };
	}

	auto BitmapDecoder::GetThumbnail() const noexcept -> ComPtr<IWICBitmapSource>
	{
		ComPtr<IWICBitmapSource> thumbnail;

		HRESULT hr = GetHeldComPtr()->GetThumbnail(&thumbnail);  HR_L(hr);

		return thumbnail;
	}
}
