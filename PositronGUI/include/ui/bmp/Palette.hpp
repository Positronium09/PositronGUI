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
		explicit Palette(const BitmapDecoder& img);
		explicit Palette(const BitmapSource& img);
		explicit Palette(std::span<WICColor> colors);
		explicit Palette(const ComPtr<IWICBitmapDecoder>& bitmapDecoder);
		explicit Palette(const ComPtr<IWICBitmapSource>& bitmapSource);

		[[nodiscard]] auto GetColors() const noexcept -> std::vector<WICColor>;
		[[nodiscard]] auto GetColorCount() const noexcept -> UINT;

		[[nodiscard]] auto GetPaletteType() const noexcept -> WICBitmapPaletteType;

		[[nodiscard]] auto IsBlackWhite() const noexcept -> bool;
		[[nodiscard]] auto IsGrayScale() const noexcept -> bool;
		[[nodiscard]] auto HasAlpha() const noexcept -> bool;
	};
}
