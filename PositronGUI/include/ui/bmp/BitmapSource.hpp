#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Rect.hpp"
#include "core/Size.hpp"

#include <span>
#include <d2d1.h>
#include <wincodec.h>


namespace PGUI::UI::Bmp
{
	class Palette;

	using BitmapResolution = Size<double>;

	class BitmapSource : public ComPtrHolder<IWICBitmapSource>
	{
		public:
		BitmapSource() noexcept = default;
		explicit BitmapSource(ComPtr<IWICBitmapSource> bmp) noexcept;

		SizeU GetSize() const noexcept;
		BitmapResolution GetResolution() const noexcept;
		WICPixelFormatGUID GetPixelFormat() const noexcept;
		void CopyPixels(RectI copyRect, UINT stride, std::span<BYTE> buffer) const;
		Palette CopyPalette() const;
		void CopyPalette(Palette palette) const noexcept;

		[[nodiscard]] ComPtr<ID2D1Bitmap> ConvertToD2D1Bitmap(ComPtr<ID2D1RenderTarget> renderTarget) const noexcept;
	};
}
