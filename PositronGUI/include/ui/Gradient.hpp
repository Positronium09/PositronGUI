#pragma once

#include "core/Rect.hpp"
#include "core/Point.hpp"
#include "core/Ellipse.hpp"
#include "PositioningMode.hpp"
#include "Color.hpp"

#include <vector>


namespace PGUI::UI
{
	struct GradientStop : public D2D1_GRADIENT_STOP
	{
		GradientStop(float _position, RGBA _color) noexcept : D2D1_GRADIENT_STOP()
		{
			position = _position;
			color = _color;
		}

		explicit(false) GradientStop(D2D1_GRADIENT_STOP gradientStop) noexcept : D2D1_GRADIENT_STOP()
		{
			position = gradientStop.position;
			color = gradientStop.color;
		}
	};

	using GradientStops = std::vector<GradientStop>;

	class Gradient
	{
		public:
		virtual ~Gradient() noexcept = default;

		[[nodiscard]] auto GetGradientStops() const noexcept -> const GradientStops&;
		[[nodiscard]] auto GetGradientStops() noexcept -> GradientStops&;

		[[nodiscard]] auto GetPositioningMode() const noexcept -> PositioningMode;
		void SetPositioningMode(PositioningMode mode) noexcept;

		virtual void ApplyReferenceRect(RectF rect) noexcept = 0;

		protected:
		explicit Gradient(GradientStops  stops) noexcept;

		private:
		GradientStops stops;
		PositioningMode mode = PositioningMode::Relative;
	};

	class LinearGradient : public Gradient
	{
		public:
		LinearGradient(PointF start, PointF end, const GradientStops& stops) noexcept;

		[[nodiscard]] auto Start() const noexcept -> PointF;
		void Start(PointF _start) noexcept;
		[[nodiscard]] auto End() const noexcept -> PointF;
		void End(PointF _end) noexcept;

		void ApplyReferenceRect(RectF rect) noexcept override;
		[[nodiscard]] auto ReferenceRectApplied(RectF rect) const noexcept -> LinearGradient;

		private:
		PointF start;
		PointF end;
	};

	class RadialGradient : public Gradient
	{
		public:
		RadialGradient(Ellipse ellipse, PointF offset, const GradientStops& stops) noexcept;

		[[nodiscard]] auto GetEllipse() const noexcept -> Ellipse;
		[[nodiscard]] auto GetEllipse() noexcept -> Ellipse&;
		void SetEllipse(Ellipse ellipse) noexcept;
		[[nodiscard]] auto Offset() const noexcept -> PointF;
		void Offset(PointF offset) noexcept;

		void ApplyReferenceRect(RectF rect) noexcept override;
		[[nodiscard]] auto ReferenceRectApplied(RectF rect) const noexcept -> RadialGradient;

		private:
		Ellipse ellipse;
		PointF offset;
	};
}
