#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "RenderTarget.hpp"
#include "GraphicsBitmap.hpp"

#include <d2d1.h>


namespace PGUI::Graphics
{
	class BitmapRenderTarget : public RenderTarget
	{
		public:
		BitmapRenderTarget() noexcept;
		explicit BitmapRenderTarget(ComPtr<ID2D1BitmapRenderTarget> brt) noexcept;

		GraphicsBitmap GetBitmap() const;
	};
}
