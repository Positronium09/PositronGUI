#pragma once

#include "Image.hpp"
#include "helpers/ComPtrHolder.hpp"

#include <d2d1.h>
#include <wincodec.h>


namespace PGUI::UI::Img
{
	class ImageMetadataReader;

	class Frame : public ComPtrHolder<IWICBitmapFrameDecode>
	{
		public:
		Frame(Image image, UINT frameIndex) noexcept;
		Frame() noexcept;

		[[nodiscard]] ImageMetadataReader GetMetadata() const noexcept;
		[[nodiscard]] ComPtr<ID2D1Bitmap> ConvertToD2D1Bitmap(ComPtr<ID2D1RenderTarget> renderTarget) const noexcept;
	};
}
