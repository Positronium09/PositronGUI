#include <utility>

#include "graphics/GraphicsBitmap.hpp"
#include "graphics/RenderTarget.hpp"


namespace PGUI::Graphics
{
	GraphicsBitmap::GraphicsBitmap() noexcept : 
		ComPtrHolder{ nullptr }
	{
	}
	GraphicsBitmap::GraphicsBitmap(ComPtr<ID2D1Bitmap> bmp) noexcept :
		ComPtrHolder{ std::move(bmp) }
	{
	}
	void GraphicsBitmap::CopyFromBitmap(const GraphicsBitmap& bmp, std::optional<PointU> destPoint, std::optional<RectU> srcRect) const
	{
		const D2D1_POINT_2U* dest = nullptr;
		const D2D1_RECT_U* src = nullptr;

		if (destPoint.has_value())
		{
			dest = std::bit_cast<const D2D1_POINT_2U*>(&(*destPoint));
		}
		if (srcRect.has_value())
		{
			src = std::bit_cast<const D2D1_RECT_U*>(&(*srcRect));
		}

		HRESULT hr = GetHeldComPtr()->CopyFromBitmap(dest, bmp, src); HR_T(hr);
	}
	void GraphicsBitmap::CopyFromMemory(const void* data, UINT32 pitch, std::optional<RectU> destRect) const
	{
		const D2D1_RECT_U* dest = nullptr;
		if (destRect.has_value())
		{
			dest = std::bit_cast<const D2D1_RECT_U*>(&(*destRect));
		}
		HRESULT hr = GetHeldComPtr()->CopyFromMemory(dest, data, pitch); HR_T(hr);
	}
	void GraphicsBitmap::CopyFromRenderTarget(const RenderTarget& rt, std::optional<PointU> destPoint, std::optional<RectU> srcRect) const
	{
		const D2D1_POINT_2U* dest = nullptr;
		const D2D1_RECT_U* src = nullptr;

		if (destPoint.has_value())
		{
			dest = std::bit_cast<const D2D1_POINT_2U*>(&(*destPoint));
		}
		if (srcRect.has_value())
		{
			src = std::bit_cast<const D2D1_RECT_U*>(&(*srcRect));
		}

		HRESULT hr = GetHeldComPtr()->CopyFromRenderTarget(dest, rt, src); HR_T(hr);
	}

	auto GraphicsBitmap::GetDPI() const noexcept -> SizeF
	{
		SizeF dpi{ };
		GetHeldComPtr()->GetDpi(&dpi.cx, &dpi.cy);
		return dpi;
	}
}
