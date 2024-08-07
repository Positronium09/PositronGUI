#include "ui/bmp/BitmapSource.hpp"

#include "ui/bmp/Palette.hpp"
#include "factories/WICFactory.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI::Bmp
{
	BitmapSource::BitmapSource(ComPtr<IWICBitmapSource> bmp) noexcept : 
		ComPtrHolder{ bmp }
	{
	}
	
	SizeU BitmapSource::GetSize() const noexcept
	{
		SizeU size{ };

		HRESULT hr = GetHeldComPtr()->GetSize(&size.cx, &size.cy); HR_L(hr);

		return size;
	}
	
	BitmapResolution BitmapSource::GetResolution() const noexcept
	{
		BitmapResolution resolution{ };

		HRESULT hr = GetHeldComPtr()->GetResolution(&resolution.cx, &resolution.cy); HR_L(hr);

		return resolution;
	}
	
	WICPixelFormatGUID BitmapSource::GetPixelFormat() const noexcept
	{
		WICPixelFormatGUID pixelFormat{ };
		
		HRESULT hr = GetHeldComPtr()->GetPixelFormat(&pixelFormat); HR_L(hr);

		return pixelFormat;
	}
	
	void BitmapSource::CopyPixels(RectI copyRect, UINT stride, std::span<BYTE> buffer) const
	{
		WICRect rc{ };
		rc.X = copyRect.left;
		rc.Y = copyRect.top;
		rc.Width = copyRect.Size().cx;
		rc.Height = copyRect.Size().cy;

		HRESULT hr = GetHeldComPtr()->CopyPixels(&rc, stride, 
			static_cast<UINT>(buffer.size_bytes()), buffer.data()); HR_L(hr);
	}

	Palette BitmapSource::CopyPalette() const
	{
		return Palette{ *this };
	}

	void BitmapSource::CopyPalette(Palette palette) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->CopyPalette(palette); HR_L(hr);
	}

	ComPtr<ID2D1Bitmap> BitmapSource::ConvertToD2D1Bitmap(ComPtr<ID2D1RenderTarget> renderTarget) const noexcept
	{
		// DXGI_FORMAT_B8G8R8A8_UNORM D2D1_ALPHA_MODE_PREMULTIPLIED

		auto wicFactory = PGUI::WICFactory::GetFactory();

		ComPtr<IWICFormatConverter> converterBitmapSource;
		HRESULT hr = wicFactory->CreateFormatConverter(&converterBitmapSource);

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
		renderTarget->CreateBitmapFromWicBitmap(converterBitmapSource.Get(), &bmp);

		return bmp;
	}
}
