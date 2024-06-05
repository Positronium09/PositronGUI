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
		explicit Bitmap(ComPtr<IWICBitmap> bmp) noexcept;

		void SetPalette(Palette palette) const;
		void SetResolution(BitmapResolution resolution) const;
	};
}
