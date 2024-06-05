#include "ui/Brush.hpp"

#include "ui/UIComponent.hpp"
#include "core/Logger.hpp"
#include "core/Exceptions.hpp"


namespace PGUI::UI
{
	SolidColorBrush::SolidColorBrush(ComPtr<ID2D1SolidColorBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	SolidColorBrush::SolidColorBrush(ComPtr<ID2D1DeviceContext> renderTarget, RGBA color)
	{
		HRESULT hr = renderTarget->CreateSolidColorBrush(color, GetHeldPtrAddress()); HR_T(hr);
	}

	ComPtr<ID2D1Brush> SolidColorBrush::GetBrush()
	{
		return GetHeldComPtr();
	}

	ID2D1Brush* SolidColorBrush::GetBrushPtr()
	{
		return GetHeldPtr();
	}


	LinearGradientBrush::LinearGradientBrush(ComPtr<ID2D1LinearGradientBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	LinearGradientBrush::LinearGradientBrush(ComPtr<ID2D1DeviceContext> renderTarget, LinearGradient gradient,
		std::optional<RectF> referenceRect)
	{
		if (referenceRect.has_value() && gradient.GetPositioningMode() == PositioningMode::Relative)
		{
			gradient.ApplyReferenceRect(referenceRect.value());
		}

		const auto& gradientStops = gradient.GetGradientStops();

		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;
		HRESULT hr = renderTarget->CreateGradientStopCollection(
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()), gradientStopCollection.GetAddressOf()); HR_T(hr);

		if (FAILED(hr))
		{
			return;
		}
		
		renderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(gradient.Start(), gradient.End()),
			gradientStopCollection.Get(),
			GetHeldPtrAddress()
		); HR_T(hr);
	}

	ComPtr<ID2D1Brush> LinearGradientBrush::GetBrush()
	{
		return GetHeldComPtr();
	}

	ID2D1Brush* LinearGradientBrush::GetBrushPtr()
	{
		return GetHeldPtr();
	}


	RadialGradientBrush::RadialGradientBrush(ComPtr<ID2D1RadialGradientBrush> brush) noexcept :
		ComPtrHolder{ brush }
	{ }

	RadialGradientBrush::RadialGradientBrush(ComPtr<ID2D1DeviceContext> renderTarget, RadialGradient gradient,
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
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()), gradientStopCollection.GetAddressOf()); HR_T(hr);

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

	ComPtr<ID2D1Brush> RadialGradientBrush::GetBrush()
	{
		return GetHeldComPtr();
	}

	ID2D1Brush* RadialGradientBrush::GetBrushPtr()
	{
		return GetHeldPtr();
	}


	BitmapBrush::BitmapBrush(ComPtr<ID2D1BitmapBrush1> brush) noexcept : 
		ComPtrHolder(brush)
	{
	}
	BitmapBrush::BitmapBrush(ComPtr<ID2D1DeviceContext> renderTarget, ComPtr<ID2D1Bitmap> bitmap)
	{
		HRESULT hr = renderTarget->CreateBitmapBrush(bitmap.Get(), GetHeldPtrAddress()); HR_T(hr);
	}

	ComPtr<ID2D1Brush> BitmapBrush::GetBrush()
	{
		return GetHeldComPtr();
	}
	ID2D1Brush* BitmapBrush::GetBrushPtr()
	{
		return GetHeldPtr();
	}

	Brush::Brush(ComPtr<ID2D1DeviceContext> renderTarget, const BrushParameters& _parameters) noexcept :
		parameters(_parameters)
	{
		CreateBrush(renderTarget);
	}

	Brush::Brush(const BrushParameters& _parameters) noexcept :
		parameters(_parameters)
	{
	}

	BrushBase* Brush::Get() const noexcept
	{
		return brush.get();
	}

	void Brush::SetParametersAndCreateBrush(
		ComPtr<ID2D1DeviceContext> renderTarget, const BrushParameters& _parameters) noexcept
	{
		SetParameters(_parameters);
		CreateBrush(renderTarget);
	}

	void Brush::CreateBrush(ComPtr<ID2D1DeviceContext> renderTarget) noexcept
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

	BrushParameters Brush::GetParameters() const noexcept
	{
		return parameters;
	}
	BrushParameters& Brush::GetParameters() noexcept
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
