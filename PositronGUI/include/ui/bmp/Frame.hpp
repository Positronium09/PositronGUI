#pragma once

#include "BitmapDecoder.hpp"
#include "BitmapSource.hpp"
#include "helpers/ComPtrHolder.hpp"

#include <d2d1.h>
#include <wincodec.h>


namespace PGUI::UI::Bmp
{
	class MetadataReader;

	class Frame : public ComPtrHolder<IWICBitmapFrameDecode>, public BitmapSource
	{
		public:
		Frame() noexcept = default;
		Frame(BitmapDecoder image, UINT frameIndex) noexcept;

		[[nodiscard]] MetadataReader GetMetadata() const noexcept;
	};
}
