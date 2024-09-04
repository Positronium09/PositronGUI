#include "graphics/Graphics.hpp"
#include "graphics/BitmapRenderTarget.hpp"
#include "graphics/GraphicsBitmap.hpp"
#include "ui/bmp/BitmapSource.hpp"
#include "factories/Direct2DFactory.hpp"
#include "factories/WICFactory.hpp"


using namespace PGUI::UI;

namespace PGUI::Graphics
{
	Graphics::Graphics(ComPtr<ID2D1DeviceContext7> rt) noexcept :
		ComPtrHolder{ rt }
	{
	}
	void Graphics::Clear(RGBA color) const noexcept
	{
		GetHeldComPtr()->Clear(color);
	}
	void Graphics::Clear(CBrushParametersRef brushParameters) const noexcept
	{
		auto brush = CreateBrush(brushParameters);
		RectF rect{ PointF{}, GetSize() };
		FillRect(rect, brush);
	}
	void Graphics::Clear(CBrushRef brush) const noexcept
	{
		RectF rect{ PointF{}, GetSize() };
		FillRect(rect, brush);
	}
	auto Graphics::CreateBitmap(SizeU size, const D2D1_BITMAP_PROPERTIES& props) const -> GraphicsBitmap
	{
		ComPtr<ID2D1Bitmap> bmp;
		HRESULT hr = GetHeldComPtr()->CreateBitmap(size, props, &bmp); HR_T(hr);
		return GraphicsBitmap{ bmp };
	}
	auto Graphics::CreateBitmap(SizeU size, const void* srcData, UINT32 pitch, const D2D1_BITMAP_PROPERTIES& bitmapProperties) const -> GraphicsBitmap
	{
		ComPtr<ID2D1Bitmap> bmp;
		HRESULT hr = GetHeldComPtr()->CreateBitmap(size, srcData, pitch, bitmapProperties, &bmp); HR_T(hr);
		return GraphicsBitmap{ bmp };
	}
	auto Graphics::CreateBitmap(const PGUI::UI::Bmp::BitmapSource& bmpSrc, std::optional<D2D1_BITMAP_PROPERTIES> props) const -> GraphicsBitmap
	{
		ComPtr<ID2D1Bitmap> bmp;
		auto wicFactory = PGUI::WICFactory::GetFactory();

		ComPtr<IWICFormatConverter> converterBitmapSource;
		HRESULT hr = wicFactory->CreateFormatConverter(&converterBitmapSource); HR_T(hr);

		hr = converterBitmapSource->Initialize(bmpSrc,
			GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			nullptr, 0.0f, WICBitmapPaletteTypeCustom); HR_T(hr);

		if (props.has_value())
		{
			hr = GetHeldComPtr()->CreateBitmapFromWicBitmap(
				bmpSrc, *props, &bmp); HR_T(hr);
			return GraphicsBitmap{ bmp };
		}
		hr = GetHeldComPtr()->CreateBitmapFromWicBitmap(
			converterBitmapSource.Get(), &bmp); HR_T(hr);
		return GraphicsBitmap{ bmp };
	}
	auto Graphics::CreateCompatibleRenderTarget(SizeF size, SizeU pixelSize,
		PixelFormat pixelFormat, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options) const -> BitmapRenderTarget
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		HRESULT hr = GetHeldComPtr()->CreateCompatibleRenderTarget(
			size, pixelSize, pixelFormat, options, &brt); HR_T(hr);
		return BitmapRenderTarget{ brt };
	}
	auto Graphics::CreateCompatibleRenderTarget(SizeF size, SizeU pixelSize, PixelFormat pixelFormat) const -> BitmapRenderTarget
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		HRESULT hr = GetHeldComPtr()->CreateCompatibleRenderTarget(
			size, pixelSize, pixelFormat, &brt); HR_T(hr);
		return BitmapRenderTarget{ brt };
	}
	auto Graphics::CreateCompatibleRenderTarget(SizeF size, SizeU pixelSize) const -> BitmapRenderTarget
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		HRESULT hr = GetHeldComPtr()->CreateCompatibleRenderTarget(
			size, pixelSize, &brt); HR_T(hr);
		return BitmapRenderTarget{ brt };
	}
	auto Graphics::CreateCompatibleRenderTarget(SizeF size) const -> BitmapRenderTarget
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		HRESULT hr = GetHeldComPtr()->CreateCompatibleRenderTarget(
			size, &brt); HR_T(hr);
		return BitmapRenderTarget{ brt };
	}
	auto Graphics::CreateCompatibleRenderTarget() const -> BitmapRenderTarget
	{
		ComPtr<ID2D1BitmapRenderTarget> brt;
		HRESULT hr = GetHeldComPtr()->CreateCompatibleRenderTarget(&brt); HR_T(hr);
		return BitmapRenderTarget{ brt };
	}
	auto Graphics::CreateSharedBitmap(const IID& riid, void* data, std::optional<D2D1_BITMAP_PROPERTIES> bitmapProperties) const -> GraphicsBitmap
	{
		ComPtr<ID2D1Bitmap> bmp;
		const D2D1_BITMAP_PROPERTIES* props = nullptr;
		if (bitmapProperties.has_value())
		{
			props = &(*bitmapProperties);
		}
		HRESULT hr = GetHeldComPtr()->CreateSharedBitmap(riid, data, props, &bmp); HR_T(hr);
		return GraphicsBitmap{ bmp };
	}
	auto Graphics::CreateMesh() const -> ComPtr<ID2D1Mesh>
	{
		return {};
	}
	auto Graphics::CreateBrush(CBrushParametersRef brushParameters) const -> Brush
	{
		Brush brush = brushParameters;
		brush.CreateBrush(GetHeldComPtr());
		return brush;
	}
	void Graphics::CreateBrush(Brush& brush) const
	{
		brush.CreateBrush(GetHeldComPtr());
	}
	void Graphics::DrawBitmap(GraphicsBitmap bmp, OptRect destRect,
		float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode, OptRect srcRect) const noexcept
	{
		const D2D1_RECT_F* dest = nullptr;
		const D2D1_RECT_F* src = nullptr;
		if (destRect.has_value())
		{
			dest = std::bit_cast<const D2D1_RECT_F*>(&(*destRect));
		}
		if (srcRect.has_value())
		{
			src = std::bit_cast<const D2D1_RECT_F*>(&(*srcRect));
		}

		GetHeldComPtr()->DrawBitmap(bmp, dest, opacity, interpolationMode, src);
	}
	void Graphics::DrawEllipse(Ellipse ellipse, CBrushRef brush, float strokeWidth, ComPtr<ID2D1StrokeStyle> strokeStyle) const noexcept
	{
		GetHeldComPtr()->DrawEllipse(ellipse, brush, strokeWidth, strokeStyle.Get());
	}
	void Graphics::DrawGlyphRun(PointF baseLineOrigin, const DWRITE_GLYPH_RUN& glyphRun,
		CBrushRef foregroundBrush, DWRITE_MEASURING_MODE measuringMode) const noexcept
	{
		GetHeldComPtr()->DrawGlyphRun(baseLineOrigin, &glyphRun, foregroundBrush, measuringMode);
	}
	void Graphics::DrawLine(PointF p1, PointF p2, CBrushRef brush, float strokeWidth, ComPtr<ID2D1StrokeStyle> strokeStyle) const noexcept
	{
		GetHeldComPtr()->DrawLine(p1, p2, brush, strokeWidth, strokeStyle.Get());
	}
	void Graphics::DrawRect(RectF rect, CBrushRef brush, float strokeWidth, ComPtr<ID2D1StrokeStyle> strokeStyle) const noexcept
	{
		GetHeldComPtr()->DrawRectangle(rect, brush, strokeWidth, strokeStyle.Get());
	}
	void Graphics::DrawRoundedRect(RoundedRect rect, CBrushRef brush, float strokeWidth, ComPtr<ID2D1StrokeStyle> strokeStyle) const noexcept
	{
		GetHeldComPtr()->DrawRoundedRectangle(rect, brush, strokeWidth, strokeStyle.Get());
	}
	void Graphics::DrawText(std::wstring_view text, UI::TextFormat textFormat,
		RectF layoutRect, CBrushRef brush, D2D1_DRAW_TEXT_OPTIONS options, DWRITE_MEASURING_MODE measuringMode) const noexcept
	{
		GetHeldComPtr()->DrawText(text.data(), static_cast<UINT32>(text.size()),
			textFormat, layoutRect, brush, options, measuringMode);
	}
	void Graphics::DrawTextLayout(PointF origin, UI::TextLayout textLayout, CBrushRef brush, D2D1_DRAW_TEXT_OPTIONS options) const noexcept
	{
		GetHeldComPtr()->DrawTextLayout(origin, textLayout, brush, options);
	}
	void Graphics::FillEllipse(Ellipse ellipse, CBrushRef brush) const noexcept
	{
		GetHeldComPtr()->FillEllipse(ellipse, brush);
	}
	void Graphics::FillOpacityMask(GraphicsBitmap bmp, CBrushRef brush,
		OptRect destRect, OptRect srcRect, D2D1_OPACITY_MASK_CONTENT content) const noexcept
	{
		const D2D1_RECT_F* dest = nullptr;
		const D2D1_RECT_F* src = nullptr;
		if (destRect.has_value())
		{
			dest = std::bit_cast<const D2D1_RECT_F*>(&(*destRect));
		}
		if (srcRect.has_value())
		{
			src = std::bit_cast<const D2D1_RECT_F*>(&(*srcRect));
		}
		GetHeldComPtr()->FillOpacityMask(bmp, brush, content, dest, src);
	}
	void Graphics::FillRect(RectF rect, CBrushRef brush) const noexcept
	{
		GetHeldComPtr()->FillRectangle(rect, brush);
	}
	void Graphics::FillRoundedRect(RoundedRect rect, CBrushRef brush) const noexcept
	{
		GetHeldComPtr()->FillRoundedRectangle(rect, brush);
	}
	auto Graphics::GetDPI() const noexcept -> SizeF
	{
		SizeF dpi{ };
		GetHeldComPtr()->GetDpi(&dpi.cx, &dpi.cy);
		return dpi;
	}
	auto Graphics::GetTags() const noexcept -> std::pair<D2D1_TAG, D2D1_TAG>
	{
		std::pair<D2D1_TAG, D2D1_TAG> tags;
		GetHeldComPtr()->GetTags(&tags.first, &tags.second);
		return tags;
	}
	auto Graphics::GetTextRenderingParams() const noexcept -> ComPtr<IDWriteRenderingParams>
	{
		ComPtr<IDWriteRenderingParams> params;
		GetHeldComPtr()->GetTextRenderingParams(&params);
		return params;
	}
	auto Graphics::GetTransform() const noexcept -> D2D1_MATRIX_3X2_F
	{
		D2D1_MATRIX_3X2_F transform;
		GetHeldComPtr()->GetTransform(&transform);
		return transform;
	}
	auto Graphics::SaveDrawingState() const noexcept -> ComPtr<ID2D1DrawingStateBlock>
	{
		ComPtr<ID2D1DrawingStateBlock> drawingState;
		auto factory = D2DFactory::GetFactory();
		factory->CreateDrawingStateBlock(&drawingState);
		GetHeldComPtr()->SaveDrawingState(drawingState.Get());
		return drawingState;
	}
}
