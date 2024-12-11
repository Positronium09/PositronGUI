#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "BitmapSource.hpp"

#include <d2d1.h>
#include <wincodec.h>


namespace PGUI::UI::Bmp
{
	class Bitmap : public ComPtrHolder<IWICBitmap>, public BitmapSource
	{
		public:
		Bitmap() noexcept = default;
		explicit Bitmap(const ComPtr<IWICBitmap>& bmp) noexcept;
		explicit Bitmap(HBITMAP bmp, 
			WICBitmapAlphaChannelOption alphaChannelOption = WICBitmapUsePremultipliedAlpha) noexcept;
		explicit Bitmap(HICON icon) noexcept;

		void SetPalette(const Palette& palette) const;
		void SetResolution(BitmapResolution resolution) const;
	};
}
