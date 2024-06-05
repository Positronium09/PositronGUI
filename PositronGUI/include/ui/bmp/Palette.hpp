#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "ui/Color.hpp"

#include <span>
#include <vector>
#include <wincodec.h>


namespace PGUI::UI::Bmp
{
	class BitmapSource;
	class BitmapDecoder;
	class Frame;

	class Palette : public ComPtrHolder<IWICPalette>
	{
		public:
		explicit Palette(BitmapDecoder img);
		explicit Palette(BitmapSource img);
		explicit Palette(std::span<WICColor> colors);
		explicit Palette(ComPtr<IWICBitmapDecoder> bitmapDecoder);
		explicit Palette(ComPtr<IWICBitmapSource> bitmapDecoder);

		[[nodiscard]] std::vector<WICColor> GetColors() const noexcept;
		[[nodiscard]] UINT GetColorCount() const noexcept;

		[[nodiscard]] WICBitmapPaletteType GetPaletteType() const noexcept;

		[[nodiscard]] bool IsBlackWhite() const noexcept;
		[[nodiscard]] bool IsGrayScale() const noexcept;
		[[nodiscard]] bool HasAlpha() const noexcept;
	};
}
