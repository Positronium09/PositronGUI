#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Rect.hpp"
#include "core/Size.hpp"

#include <span>
#include <d2d1.h>
#include <wincodec.h>


namespace PGUI::Graphics
{
	class Graphics;
	class GraphicsBitmap;
}
namespace PGUI::UI::Bmp
{
	class Palette;

	using BitmapResolution = Size<double>;

	class BitmapSource : public ComPtrHolder<IWICBitmapSource>
	{
		public:
		BitmapSource() noexcept = default;
		explicit BitmapSource(ComPtr<IWICBitmapSource> bmp) noexcept;

		[[nodiscard]] SizeU GetSize() const noexcept;
		[[nodiscard]] BitmapResolution GetResolution() const noexcept;
		[[nodiscard]] WICPixelFormatGUID GetPixelFormat() const noexcept;
		void CopyPixels(RectI copyRect, UINT stride, std::span<BYTE> buffer) const;
		[[nodiscard]] Palette CopyPalette() const;
		void CopyPalette(Palette palette) const noexcept;

		[[nodiscard]] Graphics::GraphicsBitmap ConvertToD2D1Bitmap(PGUI::Graphics::Graphics g) const noexcept;
	};
}
