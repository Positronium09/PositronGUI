#include <utility>

#include "ui/Brush.hpp"

#include "ui/UIComponent.hpp"
#include "helpers/HelperFunctions.hpp"
#include "core/Exceptions.hpp"


namespace PGUI::UI
{
	SolidColorBrush::SolidColorBrush(ComPtr<ID2D1SolidColorBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	SolidColorBrush::SolidColorBrush(ComPtr<ID2D1RenderTarget> renderTarget, RGBA color)
	{
		HRESULT hr = renderTarget->CreateSolidColorBrush(color, GetHeldPtrAddress()); HR_T(hr);
	}

	auto SolidColorBrush::GetBrush() -> ComPtr<ID2D1Brush>
	{
		return GetHeldComPtr();
	}

	auto SolidColorBrush::GetBrushPtr() -> ID2D1Brush*
	{
		return GetHeldPtr();
	}


	LinearGradientBrush::LinearGradientBrush(ComPtr<ID2D1LinearGradientBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	LinearGradientBrush::LinearGradientBrush(ComPtr<ID2D1RenderTarget> renderTarget, LinearGradient gradient,
		std::optional<RectF> referenceRect)
	{
		if (referenceRect.has_value() && gradient.GetPositioningMode() == PositioningMode::Relative)
		{
			gradient.ApplyReferenceRect(referenceRect.value());
		}

		const auto& gradientStops = gradient.GetGradientStops();

		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;
		HRESULT hr = renderTarget->CreateGradientStopCollection(
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()),
			D2D1_GAMMA_1_0, D2D1_EXTEND_MODE_CLAMP,
			&gradientStopCollection); HR_T(hr);
		
		renderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(gradient.Start(), gradient.End()),
			gradientStopCollection.Get(),
			GetHeldPtrAddress()
		); HR_T(hr);
	}

	auto LinearGradientBrush::GetBrush() -> ComPtr<ID2D1Brush>
	{
		return GetHeldComPtr();
	}

	auto LinearGradientBrush::GetBrushPtr() -> ID2D1Brush*
	{
		return GetHeldPtr();
	}


	RadialGradientBrush::RadialGradientBrush(ComPtr<ID2D1RadialGradientBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	RadialGradientBrush::RadialGradientBrush(ComPtr<ID2D1RenderTarget> renderTarget, RadialGradient gradient,
		std::optional<RectF> referenceRect)
	{
		if (referenceRect.has_value() && gradient.GetPositioningMode() == PositioningMode::Relative)
		{
			gradient.ApplyReferenceRect(referenceRect.value());
		}

		auto xRadius = gradient.GetEllipse().xRadius;
		auto yRadius = gradient.GetEllipse().yRadius;
		const auto& gradientStops = gradient.GetGradientStops();

		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;
		HRESULT hr = renderTarget->CreateGradientStopCollection(
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()), &gradientStopCollection); HR_T(hr);

		if (FAILED(hr))
		{
			return;
		}

		renderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(gradient.GetEllipse().center, gradient.Offset(), xRadius, yRadius),
			gradientStopCollection.Get(),
			GetHeldPtrAddress()
		); HR_T(hr);
	}

	auto RadialGradientBrush::GetBrush() -> ComPtr<ID2D1Brush>
	{
		return GetHeldComPtr();
	}

	auto RadialGradientBrush::GetBrushPtr() -> ID2D1Brush*
	{
		return GetHeldPtr();
	}


	BitmapBrush::BitmapBrush(ComPtr<ID2D1BitmapBrush> brush) noexcept : 
		ComPtrHolder(brush)
	{
	}
	BitmapBrush::BitmapBrush(ComPtr<ID2D1RenderTarget> renderTarget, ComPtr<ID2D1Bitmap> bitmap)
	{
		HRESULT hr = renderTarget->CreateBitmapBrush(bitmap.Get(), GetHeldPtrAddress()); HR_T(hr);
	}

	auto BitmapBrush::GetBrush() -> ComPtr<ID2D1Brush>
	{
		return GetHeldComPtr();
	}
	auto BitmapBrush::GetBrushPtr() -> ID2D1Brush*
	{
		return GetHeldPtr();
	}

	Brush::Brush(ComPtr<ID2D1RenderTarget> renderTarget, BrushParameters  _parameters) noexcept :
		parameters(std::move(_parameters))
	{
		CreateBrush(renderTarget);
	}

	Brush::Brush(Brush&& other) noexcept : 
		brush{ std::move(other.brush) },
		parameters{ other.parameters }
	{
	}

	Brush::Brush(BrushParameters  _parameters) noexcept :
		parameters(std::move(_parameters))
	{
	}

	auto Brush::Get() const noexcept -> BrushBase*
	{
		return brush.get();
	}

	void Brush::SetParametersAndCreateBrush(
		ComPtr<ID2D1RenderTarget> renderTarget, const BrushParameters& _parameters) noexcept
	{
		SetParameters(_parameters);
		CreateBrush(renderTarget);
	}

	void Brush::CreateBrush(ComPtr<ID2D1RenderTarget> renderTarget) noexcept
	{
		ReleaseBrush();

		std::visit([this, renderTarget]<typename T>(T& params)
		{
			if constexpr (std::is_same_v<T, RGBA>)
			{
				brush = std::make_unique<SolidColorBrush>(renderTarget, params);
			}
			else if constexpr (std::is_same_v<T, LinearGradientBrushParameters>)
			{
				brush = std::make_unique<LinearGradientBrush>(renderTarget, params.gradient, params.referenceRect);
			}
			else if constexpr (std::is_same_v<T, RadialGradientBrushParameters>)
			{
				brush = std::make_unique<RadialGradientBrush>(renderTarget, params.gradient, params.referenceRect);
			}
		}, parameters);
	}
	void Brush::ReleaseBrush() noexcept
	{
		brush = nullptr;
	}

	auto Brush::GetParameters() const noexcept -> BrushParameters
	{
		return parameters;
	}
	auto Brush::GetParameters() noexcept -> BrushParameters&
	{
		return parameters;
	}
	void Brush::SetParameters(const BrushParameters& _parameters) noexcept
	{
		parameters = _parameters;
	}

	void SetGradientBrushRect(Brush& brush, RectF rect)
	{
		auto& params = brush.GetParameters();
		std::visit([rect]<typename T>(T& param)
		{
			if constexpr (IsGradientBrushParameters<T>)
			{
				param.referenceRect = rect;
			}
		}, params);
	}
}
