#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Rect.hpp"
#include "ui/Color.hpp"
#include "ui/Gradient.hpp"

#include <variant>
#include <memory>
#include <optional>
#include <d2d1_3.h>


namespace PGUI::UI
{
	class BrushBase
	{
		public:
		virtual ~BrushBase() noexcept = default;
		
		virtual [[nodiscard]] ComPtr<ID2D1Brush> GetBrush() = 0;
		virtual [[nodiscard]] ID2D1Brush* GetBrushPtr() = 0;
	};

	class SolidColorBrush : public BrushBase, public ComPtrHolder<ID2D1SolidColorBrush>
	{
		public:
		SolidColorBrush() noexcept = default;
		explicit SolidColorBrush(ComPtr<ID2D1SolidColorBrush> brush) noexcept;
		explicit SolidColorBrush(ComPtr<ID2D1DeviceContext> renderTarget, RGBA color);
		
		[[nodiscard]] ComPtr<ID2D1Brush> GetBrush() override;
		[[nodiscard]] ID2D1Brush* GetBrushPtr() override;
	};

	class LinearGradientBrush : public BrushBase, public ComPtrHolder<ID2D1LinearGradientBrush>
	{
		public:
		LinearGradientBrush() noexcept = default;
		explicit LinearGradientBrush(ComPtr<ID2D1LinearGradientBrush> brush) noexcept;
		explicit LinearGradientBrush(ComPtr<ID2D1DeviceContext> renderTarget, LinearGradient gradient,
			std::optional<RectF> referenceRect = std::nullopt);

		[[nodiscard]] ComPtr<ID2D1Brush> GetBrush() override;
		[[nodiscard]] ID2D1Brush* GetBrushPtr() override;
	};

	class RadialGradientBrush : public BrushBase, public ComPtrHolder<ID2D1RadialGradientBrush>
	{
		public:
		RadialGradientBrush() noexcept = default;
		explicit RadialGradientBrush(ComPtr<ID2D1RadialGradientBrush> brush) noexcept;
		explicit RadialGradientBrush(ComPtr<ID2D1DeviceContext> renderTarget, RadialGradient gradient,
			std::optional<RectF> referenceRect = std::nullopt);

		[[nodiscard]] ComPtr<ID2D1Brush> GetBrush() override;
		[[nodiscard]] ID2D1Brush* GetBrushPtr() override;
	};

	class BitmapBrush : public BrushBase, public ComPtrHolder<ID2D1BitmapBrush1>
	{
		public:
		BitmapBrush() noexcept = default;
		explicit BitmapBrush(ComPtr<ID2D1BitmapBrush1> brush) noexcept;
		explicit BitmapBrush(ComPtr<ID2D1DeviceContext> renderTarget, ComPtr<ID2D1Bitmap> bitmap);

		[[nodiscard]] ComPtr<ID2D1Brush> GetBrush() override;
		[[nodiscard]] ID2D1Brush* GetBrushPtr() override;
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
		~LinearGradientBrushParameters() noexcept = default;
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
		~RadialGradientBrushParameters() noexcept = default;
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
		~Brush() = default;

		Brush(ComPtr<ID2D1DeviceContext> renderTarget, const BrushParameters& parameters) noexcept;
		explicit(false) Brush(const BrushParameters& parameters) noexcept;

		[[nodiscard]] BrushBase* Get() const noexcept;
		
		void SetParametersAndCreateBrush(ComPtr<ID2D1DeviceContext> renderTarget, const BrushParameters& parameters) noexcept;

		void CreateBrush(ComPtr<ID2D1DeviceContext> renderTarget) noexcept;
		void ReleaseBrush() noexcept;

		[[nodiscard]] BrushParameters GetParameters() const noexcept;
		[[nodiscard]] BrushParameters& GetParameters() noexcept;
		void SetParameters(const BrushParameters& parameters) noexcept;

		void operator=(const BrushParameters& _parameters) noexcept { parameters = _parameters; }
		[[nodiscard]] BrushBase* operator->() const noexcept { return brush.get(); }
		[[nodiscard]] explicit(false) operator BrushBase* () const noexcept { return brush.get(); }
		[[nodiscard]] explicit operator bool() const noexcept { return (bool)brush; }

		private:
		std::unique_ptr<BrushBase> brush = nullptr;
		BrushParameters parameters;
	};

	void SetGradientBrushRect(Brush& brush, RectF rect);
}
