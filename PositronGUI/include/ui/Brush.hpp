#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Rect.hpp"
#include "ui/Color.hpp"
#include "ui/Gradient.hpp"

#include <d2d1_3.h>
#include <memory>
#include <optional>
#include <utility>
#include <variant>


namespace PGUI::UI
{
	class BrushBase
	{
		public:
		virtual ~BrushBase() noexcept = default;
		
		[[nodiscard]] virtual auto GetBrush() -> ComPtr<ID2D1Brush> = 0;
		[[nodiscard]] virtual auto GetBrushPtr() -> ID2D1Brush* = 0;
	};

	class SolidColorBrush : public BrushBase, public ComPtrHolder<ID2D1SolidColorBrush>
	{
		public:
		SolidColorBrush() noexcept = default;
		explicit SolidColorBrush(ComPtr<ID2D1SolidColorBrush> brush) noexcept;
		explicit SolidColorBrush(const ComPtr<ID2D1RenderTarget>& renderTarget, RGBA color);
		
		[[nodiscard]] auto GetBrush() -> ComPtr<ID2D1Brush> override;
		[[nodiscard]] auto GetBrushPtr() -> ID2D1Brush* override;
	};

	class LinearGradientBrush : public BrushBase, public ComPtrHolder<ID2D1LinearGradientBrush>
	{
		public:
		LinearGradientBrush() noexcept = default;
		explicit LinearGradientBrush(ComPtr<ID2D1LinearGradientBrush> brush) noexcept;
		explicit LinearGradientBrush(const ComPtr<ID2D1RenderTarget>& renderTarget, LinearGradient gradient,
			std::optional<RectF> referenceRect = std::nullopt);

		[[nodiscard]] auto GetBrush() -> ComPtr<ID2D1Brush> override;
		[[nodiscard]] auto GetBrushPtr() -> ID2D1Brush* override;
	};

	class RadialGradientBrush : public BrushBase, public ComPtrHolder<ID2D1RadialGradientBrush>
	{
		public:
		RadialGradientBrush() noexcept = default;
		explicit RadialGradientBrush(ComPtr<ID2D1RadialGradientBrush> brush) noexcept;
		explicit RadialGradientBrush(const ComPtr<ID2D1RenderTarget>& renderTarget, RadialGradient gradient,
			std::optional<RectF> referenceRect = std::nullopt);

		[[nodiscard]] auto GetBrush() -> ComPtr<ID2D1Brush> override;
		[[nodiscard]] auto GetBrushPtr() -> ID2D1Brush* override;
	};

	class BitmapBrush : public BrushBase, public ComPtrHolder<ID2D1BitmapBrush>
	{
		public:
		BitmapBrush() noexcept = default;
		explicit BitmapBrush(ComPtr<ID2D1BitmapBrush> brush) noexcept;
		explicit BitmapBrush(const ComPtr<ID2D1RenderTarget>& renderTarget, const ComPtr<ID2D1Bitmap>& bitmap);

		[[nodiscard]] auto GetBrush() -> ComPtr<ID2D1Brush> override;
		[[nodiscard]] auto GetBrushPtr() -> ID2D1Brush* override;
	};

	using SolidColorBrushParameters = RGBA;

	struct LinearGradientBrushParameters
	{
		LinearGradient gradient;
		std::optional<RectF> referenceRect;

		LinearGradientBrushParameters(
			const LinearGradient& gradient, 
			const std::optional<RectF>& referenceRect = std::nullopt) noexcept
			: gradient(gradient), referenceRect(referenceRect)
		{
		}
	};

	struct RadialGradientBrushParameters
	{
		RadialGradient gradient;
		std::optional<RectF> referenceRect;

		RadialGradientBrushParameters(
			const RadialGradient& gradient, 
			const std::optional<RectF>& referenceRect = std::nullopt) noexcept
			: gradient(gradient), referenceRect(referenceRect)
		{
		}
	};

	using BrushParameters = 
		std::variant<SolidColorBrushParameters, LinearGradientBrushParameters, RadialGradientBrushParameters>;

	template <typename T>
	concept IsGradientBrushParameters = 
		std::is_same_v<T, LinearGradientBrushParameters> || std::is_same_v<T, RadialGradientBrushParameters>;

	class Brush final
	{
		public:
		Brush() noexcept = default;
		Brush(ComPtr<ID2D1RenderTarget> renderTarget, BrushParameters  parameters) noexcept;
		Brush(const Brush&) noexcept = delete;
		Brush(Brush&& other) noexcept;
		explicit(false) Brush(BrushParameters parameters) noexcept;
		~Brush() = default;

		[[nodiscard]] auto Get() const noexcept -> BrushBase*;
		
		void SetParametersAndCreateBrush(ComPtr<ID2D1RenderTarget> renderTarget, const BrushParameters& parameters) noexcept;

		void CreateBrush(ComPtr<ID2D1RenderTarget> renderTarget) noexcept;
		void ReleaseBrush() noexcept;

		[[nodiscard]] auto GetParameters() const noexcept -> BrushParameters;
		[[nodiscard]] auto GetParameters() noexcept -> BrushParameters&;
		void SetParameters(const BrushParameters& parameters) noexcept;

		auto operator=(const BrushParameters& _parameters) noexcept -> Brush& { parameters = _parameters; return *this; }
		[[nodiscard]] auto operator->() const noexcept -> ID2D1Brush* { return brush->GetBrushPtr(); }
		[[nodiscard]] explicit(false) operator BrushBase* () const noexcept { return brush.get(); }
		[[nodiscard]] explicit(false) operator ID2D1Brush* () const noexcept { return brush->GetBrushPtr(); }
		[[nodiscard]] explicit operator bool() const noexcept { return (bool)brush; }

		private:
		std::unique_ptr<BrushBase> brush = nullptr;
		BrushParameters parameters;
	};

	void SetGradientBrushRect(Brush& brush, RectF rect);
}
