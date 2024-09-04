#include "graphics/BitmapRenderTarget.hpp"


namespace PGUI::Graphics
{
	BitmapRenderTarget::BitmapRenderTarget() noexcept : 
		RenderTarget{ nullptr }
	{
	}
	BitmapRenderTarget::BitmapRenderTarget(ComPtr<ID2D1BitmapRenderTarget> brt) noexcept :
		RenderTarget{ brt }
	{
	}

	auto BitmapRenderTarget::GetBitmap() const -> GraphicsBitmap
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		GetHeldComPtr().As(&brt);
		ComPtr<ID2D1Bitmap> bmp;
		brt->GetBitmap(&bmp);

		return GraphicsBitmap{ bmp };
	}
}
