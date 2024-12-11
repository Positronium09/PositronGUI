#include <utility>

#include "ui/Gradient.hpp"

#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI
{
	Gradient::Gradient(GradientStops  _stops) noexcept :
		stops(std::move(_stops))
	{ }

	auto Gradient::GetGradientStops() const noexcept -> const GradientStops&
	{
		return stops;
	}
	auto Gradient::GetGradientStops() noexcept -> GradientStops&
	{
		return stops;
	}

	auto Gradient::GetPositioningMode() const noexcept -> PositioningMode
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
	
	auto LinearGradient::Start() const noexcept -> PointF
	{
		return start;
	}
	void LinearGradient::Start(PointF _start) noexcept
	{
		start = _start;
	}
	
	auto LinearGradient::End() const noexcept -> PointF
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

	auto LinearGradient::ReferenceRectApplied(RectF rect) const noexcept -> LinearGradient
	{
		auto gradient = *this;
		gradient.ApplyReferenceRect(rect);

		return gradient;
	}

	RadialGradient::RadialGradient(Ellipse _ellipse, PointF _offset, const GradientStops& stops) noexcept :
		Gradient{ stops }, ellipse(_ellipse), offset(_offset)
	{ }

	auto RadialGradient::GetEllipse() const noexcept -> Ellipse
	{
		return ellipse;
	}

	auto RadialGradient::GetEllipse() noexcept -> Ellipse&
	{
		return ellipse;
	}

	void RadialGradient::SetEllipse(Ellipse _ellipse) noexcept
	{
		ellipse = _ellipse;
	}

	auto RadialGradient::Offset() const noexcept -> PointF
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

		ellipse.xRadius = MapToRange(ellipse.xRadius, 0.0F, size.cx);
		ellipse.yRadius = MapToRange(ellipse.yRadius, 0.0F, size.cy);
	}

	auto RadialGradient::ReferenceRectApplied(RectF rect) const noexcept -> RadialGradient
	{
		auto gradient = *this;
		gradient.ApplyReferenceRect(rect);

		return gradient;
	}
}
