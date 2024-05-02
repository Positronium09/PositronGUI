#include "ui/img/Frame.hpp"

#include "ui/img/ImageMetadataReader.hpp"
#include "factories/WICFactory.hpp"
#include "core/Logger.hpp"


namespace PGUI::UI::Img
{
	Frame::Frame(Image image, UINT frameIndex) noexcept
	{
		HRESULT hr = image->GetFrame(frameIndex, GetHeldComPtrAddress()); HR_L(hr);
	}

	Frame::Frame() noexcept : 
		ComPtrHolder{ nullptr }
	{
	}
	ImageMetadataReader Frame::GetMetadata() const noexcept
	{
		return ImageMetadataReader{ *this };
	}
	ComPtr<ID2D1Bitmap> Frame::ConvertToD2D1Bitmap(ComPtr<ID2D1RenderTarget> renderTarget) const noexcept
	{
		// DXGI_FORMAT_B8G8R8A8_UNORM D2D1_ALPHA_MODE_PREMULTIPLIED

		auto wicFactory = PGUI::WICFactory::GetFactory();

		ComPtr<IWICFormatConverter> converterBitmapSource;
		HRESULT hr = wicFactory->CreateFormatConverter(converterBitmapSource.GetAddressOf());
		
		if (FAILED(hr))
		{
			HR_L(hr);
			return nullptr;
		}

		hr = converterBitmapSource->Initialize(GetHeldPtr(), 
			GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			nullptr, 0.0f, WICBitmapPaletteTypeCustom);

		if (FAILED(hr))
		{
			HR_L(hr);
			return nullptr;
		}

		ComPtr<ID2D1Bitmap> bmp;
		renderTarget->CreateBitmapFromWicBitmap(converterBitmapSource.Get(), bmp.GetAddressOf());

		return bmp;
	}
}
