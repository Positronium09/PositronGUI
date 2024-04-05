#include "ui/Gradient.hpp"

#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI
{
	Gradient::Gradient(const GradientStops& _stops) noexcept :
		stops(_stops)
	{ }

	const GradientStops& Gradient::GetGradientStops() const noexcept
	{
		return stops;
	}
	GradientStops& Gradient::GetGradientStops() noexcept
	{
		return stops;
	}

	PositioningMode Gradient::GetPositioningMode() const noexcept
	{
		return mode;
	}

	void Gradient::SetPositioningMode(PositioningMode _mode) noexcept
	{
		mode = _mode;
	}

	LinearGradient::LinearGradient(PointF _start, PointF _end, const GradientStops& stops) noexcept :
		Gradient{ stops }, start(_start), end(_end)
	{ }
	
	PointF LinearGradient::Start() const noexcept
	{
		return start;
	}
	void LinearGradient::Start(PointF _start) noexcept
	{
		start = _start;
	}
	
	PointF LinearGradient::End() const noexcept
	{
		return end;
	}
	void LinearGradient::End(PointF _end) noexcept
	{
		end = _end;
	}

	void LinearGradient::ApplyReferenceRect(RectF rect) noexcept
	{
		start.x = MapToRange(start.x, rect.left, rect.right);
		start.y = MapToRange(start.y, rect.top, rect.bottom);

		end.x = MapToRange(end.x, rect.left, rect.right);
		end.y = MapToRange(end.y, rect.top, rect.bottom);
	}

	LinearGradient LinearGradient::ReferenceRectApplied(RectF rect) const noexcept
	{
		auto gradient = *this;
		gradient.ApplyReferenceRect(rect);

		return gradient;
	}

	RadialGradient::RadialGradient(Ellipse _ellipse, PointF _offset, const GradientStops& stops) noexcept :
		Gradient{ stops }, ellipse(_ellipse), offset(_offset)
	{ }

	Ellipse RadialGradient::GetEllipse() const noexcept
	{
		return ellipse;
	}

	Ellipse& RadialGradient::GetEllipse() noexcept
	{
		return ellipse;
	}

	void RadialGradient::SetEllipse(Ellipse _ellipse) noexcept
	{
		ellipse = _ellipse;
	}

	PointF RadialGradient::Offset() const noexcept
	{
		return offset;
	}
	void RadialGradient::Offset(PointF _offset) noexcept
	{
		offset = _offset;
	}

	void RadialGradient::ApplyReferenceRect(RectF rect) noexcept
	{
		auto size = rect.Size();

		ellipse.center.x = MapToRange(ellipse.center.x, rect.right, rect.left);
		ellipse.center.y = MapToRange(ellipse.center.y, rect.bottom, rect.top);

		offset.x = MapToRange(offset.y, rect.right, rect.left);
		offset.y = MapToRange(offset.y, rect.bottom, rect.top);

		ellipse.xRadius = MapToRange(ellipse.xRadius, 0.0f, size.cx);
		ellipse.yRadius = MapToRange(ellipse.yRadius, 0.0f, size.cy);
	}

	RadialGradient RadialGradient::ReferenceRectApplied(RectF rect) const noexcept
	{
		auto gradient = *this;
		gradient.ApplyReferenceRect(rect);

		return gradient;
	}
}
