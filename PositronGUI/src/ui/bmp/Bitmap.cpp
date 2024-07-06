#include "ui/bmp/Bitmap.hpp"

#include "factories/WICFactory.hpp"
#include "ui/bmp/Palette.hpp"
#include "core/Logger.hpp"


namespace PGUI::UI::Bmp
{
	Bitmap::Bitmap(ComPtr<IWICBitmap> bmp) noexcept : 
		ComPtrHolder<IWICBitmap>{ bmp }, BitmapSource{ bmp }
	{
		
	}

	Bitmap::Bitmap(HBITMAP bmp, WICBitmapAlphaChannelOption alphaChannelOption) noexcept
	{
		auto factory = WICFactory::GetFactory();
		HRESULT hr = factory->CreateBitmapFromHBITMAP(bmp, nullptr, alphaChannelOption,
			ComPtrHolder<IWICBitmap>::GetHeldComPtrAddress()); HR_T(hr);
		ComPtrHolder<IWICBitmap>::GetHeldComPtr().As(ComPtrHolder<IWICBitmapSource>::GetHeldComPtrAddress());
	}

	Bitmap::Bitmap(HICON icon) noexcept
	{
		auto factory = WICFactory::GetFactory();
		HRESULT hr = factory->CreateBitmapFromHICON(icon, 
			ComPtrHolder<IWICBitmap>::GetHeldComPtrAddress()); HR_T(hr);
		ComPtrHolder<IWICBitmap>::GetHeldComPtr().As(ComPtrHolder<IWICBitmapSource>::GetHeldComPtrAddress());
	}

	void Bitmap::SetPalette(Palette palette) const
	{
		auto ptr = ComPtrHolder<IWICBitmap>::GetHeldComPtr();
		HRESULT hr = ptr->SetPalette(palette); HR_T(hr);
	}

	void Bitmap::SetResolution(BitmapResolution resolution) const
	{
		auto ptr = ComPtrHolder<IWICBitmap>::GetHeldComPtr();
		HRESULT hr = ptr->SetResolution(resolution.cx, resolution.cy); HR_T(hr);
	}
}
