#include "ui/bmp/Bitmap.hpp"

#include "ui/bmp/Palette.hpp"
#include "core/Logger.hpp"


namespace PGUI::UI::Bmp
{
	Bitmap::Bitmap(ComPtr<IWICBitmap> bmp) noexcept : 
		ComPtrHolder<IWICBitmap>{ bmp }, BitmapSource{ bmp }
	{
		
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
