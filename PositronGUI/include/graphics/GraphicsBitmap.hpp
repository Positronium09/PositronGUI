#pragma once

#include "core/Rect.hpp"
#include "core/Point.hpp"
#include "core/Size.hpp"
#include "helpers/ComPtrHolder.hpp"
#include "graphics/PixelFormat.hpp"

#include <optional>
#include <d2d1.h>


namespace PGUI::Graphics
{
	class RenderTarget;

	class GraphicsBitmap : public ComPtrHolder<ID2D1Bitmap>
	{
		public:
		explicit GraphicsBitmap() noexcept;
		explicit GraphicsBitmap(ComPtr<ID2D1Bitmap> bmp) noexcept;

		void CopyFromBitmap(GraphicsBitmap bmp, 
			std::optional<PointU> destPoint = std::nullopt, 
			std::optional<RectU> srcRect = std::nullopt) const;

		void CopyFromMemory(const void* data, UINT32 pitch,
			std::optional<RectU> destRect = std::nullopt) const;

		void CopyFromRenderTarget(RenderTarget rt,
			std::optional<PointU> destPoint = std::nullopt,
			std::optional<RectU> srcRect = std::nullopt) const;

		[[nodiscard]] SizeF GetDPI() const noexcept;
		[[nodiscard]] PixelFormat GetPixelFormat() const noexcept { return GetHeldPtr()->GetPixelFormat(); }
		[[nodiscard]] SizeU GetPixelSize() const noexcept { return GetHeldPtr()->GetPixelSize(); }
		[[nodiscard]] SizeF GetSize() const noexcept { return GetHeldPtr()->GetSize(); }
	};
}
