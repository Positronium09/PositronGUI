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
		GradientStop(float _position, RGBA _color) noexcept
		{
			position = _position;
			color = _color;
		}

		explicit(false) GradientStop(D2D1_GRADIENT_STOP gradientStop) noexcept
		{
			position = gradientStop.position;
			color = gradientStop.color;
		}

		explicit(false) operator D2D1_GRADIENT_STOP() const noexcept
		{
			return *this;
		}
	};

	using GradientStops = std::vector<GradientStop>;

	class Gradient
	{
		public:
		virtual ~Gradient() noexcept = default;

		[[nodiscard]] const GradientStops& GetGradientStops() const noexcept;
		[[nodiscard]] GradientStops& GetGradientStops() noexcept;

		[[nodiscard]] PositioningMode GetPositioningMode() const noexcept;
		void SetPositioningMode(PositioningMode mode) noexcept;

		virtual void ApplyReferenceRect(RectF rect) noexcept = 0;

		protected:
		explicit Gradient(const GradientStops& stops) noexcept;

		private:
		GradientStops stops;
		PositioningMode mode = PositioningMode::Relative;
	};

	class LinearGradient : public Gradient
	{
		public:
		LinearGradient(PointF start, PointF end, const GradientStops& stops) noexcept;

		[[nodiscard]] PointF Start() const noexcept;
		void Start(PointF center) noexcept;
		[[nodiscard]] PointF End() const noexcept;
		void End(PointF offset) noexcept;

		void ApplyReferenceRect(RectF rect) noexcept override;
		[[nodiscard]] LinearGradient ReferenceRectApplied(RectF rect) const noexcept;

		private:
		PointF start;
		PointF end;
	};

	class RadialGradient : public Gradient
	{
		public:
		RadialGradient(Ellipse ellipse, PointF offset, const GradientStops& stops) noexcept;

		[[nodiscard]] Ellipse GetEllipse() const noexcept;
		[[nodiscard]] Ellipse& GetEllipse() noexcept;
		void SetEllipse(Ellipse ellipse) noexcept;
		[[nodiscard]] PointF Offset() const noexcept;
		void Offset(PointF offset) noexcept;

		void ApplyReferenceRect(RectF rect) noexcept override;
		[[nodiscard]] RadialGradient ReferenceRectApplied(RectF rect) const noexcept;

		private:
		Ellipse ellipse;
		PointF offset;
	};
}
