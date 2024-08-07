#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Rect.hpp"
#include "core/Size.hpp"
#include "core/Point.hpp"
#include "core/Ellipse.hpp"
#include "core/RoundedRect.hpp"
#include "ui/Color.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"
#include "ui/Brush.hpp"
#include "ui/bmp/BitmapSource.hpp"

#include "AntialiasMode.hpp"
#include "PixelFormat.hpp"

#include <optional>
#include <d2d1_3.h>


namespace PGUI::Graphics
{
	class GraphicsBitmap;
	class BitmapRenderTarget;
	//? Yes code duplication what you gonna do

	class Graphics : public ComPtrHolder<ID2D1DeviceContext7>
	{
		using OptRect = std::optional<RectF>;
		using RGBA = PGUI::UI::RGBA;
		using BrushParameters = PGUI::UI::BrushParameters;
		using CBrushParametersRef = const BrushParameters&;
		using Brush = PGUI::UI::Brush;
		using CBrushRef = const Brush&;

		public:
		explicit Graphics(ComPtr<ID2D1DeviceContext7> rt) noexcept;

		void Clear(RGBA color) const noexcept;
		void Clear(CBrushParametersRef brushParameters) const noexcept;
		void Clear(CBrushRef brush) const noexcept;

		[[nodiscard]] GraphicsBitmap CreateBitmap(SizeU size, const D2D1_BITMAP_PROPERTIES& props) const;
		[[nodiscard]] GraphicsBitmap CreateBitmap(SizeU size, const void* srcData, UINT32 pitch,
			const D2D1_BITMAP_PROPERTIES& bitmapProperties) const;
		[[nodiscard]] GraphicsBitmap CreateBitmap(PGUI::UI::Bmp::BitmapSource bmpSrc,
			std::optional<D2D1_BITMAP_PROPERTIES> props = std::nullopt) const;

		[[nodiscard]] BitmapRenderTarget CreateCompatibleRenderTarget(
			SizeF size, SizeU pixelSize, PixelFormat pixelFormat,
			D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options) const;
		[[nodiscard]] BitmapRenderTarget CreateCompatibleRenderTarget(
			SizeF size, SizeU pixelSize, PixelFormat pixelFormat) const;
		[[nodiscard]] BitmapRenderTarget CreateCompatibleRenderTarget(
			SizeF size, SizeU pixelSize) const;
		[[nodiscard]] BitmapRenderTarget CreateCompatibleRenderTarget(SizeF size) const;
		[[nodiscard]] BitmapRenderTarget CreateCompatibleRenderTarget() const;

		[[nodiscard]] GraphicsBitmap CreateSharedBitmap(
			const IID& riid, void* data,
			std::optional<D2D1_BITMAP_PROPERTIES> bitmapProperties) const;

		ComPtr<ID2D1Mesh> CreateMesh() const;

		[[nodiscard]] Brush CreateBrush(CBrushParametersRef brushParameters) const;
		void CreateBrush(Brush& brush) const;

		void DrawBitmap(GraphicsBitmap bmp,
			OptRect  destRect = std::nullopt,
			float opacity = 1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			OptRect  srcRect = std::nullopt) const noexcept;

		void DrawEllipse(Ellipse ellipse, CBrushRef brush,
			float strokeWidth = 1.0f, ComPtr<ID2D1StrokeStyle> strokeStyle = nullptr) const noexcept;

		void DrawGlyphRun(PointF baseLineOrigin,
			const DWRITE_GLYPH_RUN& glyphRun, CBrushRef foregroundBrush,
			DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL) const noexcept;

		void DrawLine(PointF p1, PointF p2, CBrushRef brush,
			float strokeWidth = 1.0f, ComPtr<ID2D1StrokeStyle> strokeStyle = nullptr) const noexcept;

		void DrawRect(RectF rect, CBrushRef brush,
			float strokeWidth = 1.0f, ComPtr<ID2D1StrokeStyle> strokeStyle = nullptr) const noexcept;

		void DrawRoundedRect(RoundedRect rect, CBrushRef brush,
			float strokeWidth = 1.0f, ComPtr<ID2D1StrokeStyle> strokeStyle = nullptr) const noexcept;

		void DrawText(std::wstring_view text, UI::TextFormat textFormat,
			RectF layoutRect, CBrushRef brush,
			D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE,
			DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL) const noexcept;
		void DrawTextLayout(PointF origin, UI::TextLayout textLayout,
			CBrushRef brush, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE) const noexcept;

		void FillEllipse(Ellipse ellipse, CBrushRef brush) const noexcept;

		void FillOpacityMask(GraphicsBitmap bmp, CBrushRef brush,
			OptRect destRect = std::nullopt, OptRect srcRect = std::nullopt,
			D2D1_OPACITY_MASK_CONTENT content = D2D1_OPACITY_MASK_CONTENT_GRAPHICS) const noexcept;

		void FillRect(RectF rect, CBrushRef brush) const noexcept;

		void FillRoundedRect(RoundedRect rect, CBrushRef brush) const noexcept;

		[[nodiscard]] auto GetAntialiasMode() const noexcept
		{
			return static_cast<AntialiasMode>(GetHeldComPtr()->GetAntialiasMode());
		}

		[[nodiscard]] SizeF GetDpi() const noexcept;

		[[nodiscard]] auto GetMaximumBitmapSize() const noexcept { return GetHeldComPtr()->GetMaximumBitmapSize(); }
		[[nodiscard]] PixelFormat GetPixelFormat() const noexcept { return GetHeldComPtr()->GetPixelFormat(); }
		[[nodiscard]] SizeU GetPixelSize() const noexcept { return GetHeldComPtr()->GetPixelSize(); }
		[[nodiscard]] SizeF GetSize() const noexcept { return GetHeldComPtr()->GetSize(); }

		[[nodiscard]] std::pair<D2D1_TAG, D2D1_TAG> GetTags() const noexcept;
		[[nodiscard]] auto GetTextAntialiasMode() const noexcept
		{
			return GetHeldComPtr()->GetTextAntialiasMode();
		}

		[[nodiscard]] ComPtr<IDWriteRenderingParams> GetTextRenderingParams() const noexcept;

		[[nodiscard]] bool IsSupported(const D2D1_RENDER_TARGET_PROPERTIES& props) const noexcept
		{
			return GetHeldComPtr()->IsSupported(props);
		}

		[[nodiscard]] D2D1_MATRIX_3X2_F GetTransform() const noexcept;
		void PopAxisAlignedClip() const noexcept { GetHeldComPtr()->PopAxisAlignedClip(); }
		void PopLayer() const noexcept { GetHeldComPtr()->PopLayer(); }

		void PushAxisAlignedClip(RectF rect, AntialiasMode antialiasMode) const noexcept
		{
			GetHeldComPtr()->PushAxisAlignedClip(rect, static_cast<D2D1_ANTIALIAS_MODE>(antialiasMode));
		}

		void PushLayer(const D2D1_LAYER_PARAMETERS& layerParameters, ComPtr<ID2D1Layer> layer) const noexcept
		{
			GetHeldComPtr()->PushLayer(layerParameters, layer.Get());
		}

		void RestoreDrawingState(ComPtr<ID2D1DrawingStateBlock> drawingStateBlock) const noexcept
		{
			GetHeldComPtr()->RestoreDrawingState(drawingStateBlock.Get());
		}
		[[nodiscard]] ComPtr<ID2D1DrawingStateBlock> SaveDrawingState() const noexcept;

		void SetAntialiasMode(AntialiasMode antialiasMode) const noexcept
		{
			GetHeldComPtr()->SetAntialiasMode(static_cast<D2D1_ANTIALIAS_MODE>(antialiasMode));
		}

		void SetDpi(SizeF dpi) const noexcept
		{
			GetHeldComPtr()->SetDpi(dpi.cx, dpi.cy);
		}

		void SetTags(D2D1_TAG tag1, D2D1_TAG tag2) const noexcept
		{
			GetHeldComPtr()->SetTags(tag1, tag2);
		}
		void SetTags(const std::pair<D2D1_TAG, D2D1_TAG>& tags) const noexcept
		{
			SetTags(tags.first, tags.second);
		}

		void SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode) const noexcept
		{
			GetHeldComPtr()->SetTextAntialiasMode(textAntialiasMode);
		}

		void SetTextRenderingParams(ComPtr<IDWriteRenderingParams> textRenderingParams) const noexcept
		{
			GetHeldComPtr()->SetTextRenderingParams(textRenderingParams.Get());
		}
		void ClearTextRenderingParams() const noexcept
		{
			SetTextRenderingParams(nullptr);
		}

		void SetTransform(const D2D1_MATRIX_3X2_F& transform) const noexcept
		{
			GetHeldComPtr()->SetTransform(transform);
		}
	};
}
