#include <utility>

#include "ui/bmp/BitmapSource.hpp"

#include "graphics/GraphicsBitmap.hpp"
#include "graphics/Graphics.hpp"
#include "ui/bmp/Palette.hpp"
#include "factories/WICFactory.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI::Bmp
{
	BitmapSource::BitmapSource(ComPtr<IWICBitmapSource> bmp) noexcept : 
		ComPtrHolder{ std::move(bmp) }
	{
	}
	
	auto BitmapSource::GetSize() const noexcept -> SizeU
	{
		SizeU size{ };

		HRESULT hr = GetHeldComPtr()->GetSize(&size.cx, &size.cy); HR_L(hr);

		return size;
	}
	
	auto BitmapSource::GetResolution() const noexcept -> BitmapResolution
	{
		BitmapResolution resolution{ };

		HRESULT hr = GetHeldComPtr()->GetResolution(&resolution.cx, &resolution.cy); HR_L(hr);

		return resolution;
	}
	
	auto BitmapSource::GetPixelFormat() const noexcept -> WICPixelFormatGUID
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

	auto BitmapSource::CopyPalette() const -> Palette
	{
		return Palette{ *this };
	}

	void BitmapSource::CopyPalette(const Palette& palette) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->CopyPalette(palette); HR_L(hr);
	}

	auto BitmapSource::ConvertToD2D1Bitmap(const Graphics::Graphics& g) const noexcept -> Graphics::GraphicsBitmap
	{
		// DXGI_FORMAT_B8G8R8A8_UNORM D2D1_ALPHA_MODE_PREMULTIPLIED
		return g.CreateBitmap(*this);
	}
}
