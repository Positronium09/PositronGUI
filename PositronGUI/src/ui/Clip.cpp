#include <utility>

#include "ui/Clip.hpp"

#include "core/Window.hpp"
#include "helpers/HelperFunctions.hpp"
#include "factories/Direct2DFactory.hpp"


namespace PGUI::UI
{
	auto EmptyClip::GetClipGeometry() -> ComPtr<ID2D1Geometry>
	{
		return nullptr;
	}
	auto EmptyClip::GetClipGeometryPtr() -> ID2D1Geometry*
	{
		return nullptr;
	}

	RectangleClip::RectangleClip(ComPtr<ID2D1RectangleGeometry> geometry) noexcept : 
		ComPtrHolder{ geometry }
	{
	}

	RectangleClip::RectangleClip(RectF rect)
	{
		auto factory = D2DFactory::GetFactory();

		HRESULT hr = factory->CreateRectangleGeometry(rect, GetHeldPtrAddress()); HR_T(hr);
	}

	auto RectangleClip::GetClipGeometry() -> ComPtr<ID2D1Geometry>
	{
		return GetHeldComPtr();
	}
	auto RectangleClip::GetClipGeometryPtr() -> ID2D1Geometry*
	{
		return GetHeldPtr();
	}

	
	RoundedRectangleClip::RoundedRectangleClip(ComPtr<ID2D1RoundedRectangleGeometry> geometry) noexcept : 
		ComPtrHolder{ geometry }
	{
	}
	RoundedRectangleClip::RoundedRectangleClip(RoundedRect roundedRect)
	{
		auto factory = D2DFactory::GetFactory();

		HRESULT hr = factory->CreateRoundedRectangleGeometry(roundedRect, GetHeldPtrAddress()); HR_T(hr);
	}

	auto RoundedRectangleClip::GetClipGeometry() -> ComPtr<ID2D1Geometry>
	{
		return GetHeldComPtr();
	}
	auto RoundedRectangleClip::GetClipGeometryPtr() -> ID2D1Geometry*
	{
		return GetHeldPtr();
	}


	EllipseClip::EllipseClip(ComPtr<ID2D1EllipseGeometry> geometry) noexcept : 
		ComPtrHolder{ geometry }
	{
	}
	EllipseClip::EllipseClip(PGUI::Ellipse ellipse)
	{
		auto factory = D2DFactory::GetFactory();

		HRESULT hr = factory->CreateEllipseGeometry(ellipse, GetHeldComPtrAddress()); HR_T(hr);
	}

	auto EllipseClip::GetClipGeometry() -> ComPtr<ID2D1Geometry>
	{
		return GetHeldComPtr();
	}
	auto EllipseClip::GetClipGeometryPtr() -> ID2D1Geometry*
	{
		return GetHeldPtr();
	}


	PathClip::PathClip(ComPtr<ID2D1PathGeometry1> geometry) noexcept : 
		ComPtrHolder{ geometry }
	{
		
	}
	auto PathClip::GetClipGeometry() -> ComPtr<ID2D1Geometry>
	{
		return GetHeldComPtr();
	}
	auto PathClip::GetClipGeometryPtr() -> ID2D1Geometry*
	{
		return GetHeldPtr();
	}

	Clip::Clip(ClipParameters  _parameters) noexcept : 
		parameters(std::move(_parameters))
	{
		CreateClip();
	}
	auto Clip::Get() const noexcept -> ClipBase*
	{
		return clip.get();
	}
	
	void Clip::CreateClip() noexcept
	{
		ReleaseClip();

		std::visit([this]<typename T>(T& parameter)
		{
			if constexpr (std::is_same_v<T, EmptyClipParameters>)
			{
				clip = std::make_unique<EmptyClip>();
			}
			else if constexpr (std::is_same_v<T, RectangleClipParameters>)
			{
				clip = std::make_unique<RectangleClip>(parameter.rect);
			}
			else if constexpr (std::is_same_v<T, RoundedRectangleClipParameters>)
			{
				clip = std::make_unique<RoundedRectangleClip>(parameter.roundedRect);
			}
			else if constexpr (std::is_same_v<T, EllipseClipParameters>)
			{
				clip = std::make_unique<EllipseClip>(parameter.ellipse);
			}
		}, parameters);
	}
	void Clip::ReleaseClip() noexcept
	{
		clip.reset();
	}

	auto Clip::GetParameters() const noexcept -> ClipParameters
	{
		return parameters;
	}
	auto Clip::GetParameters() noexcept -> ClipParameters&
	{
		return parameters;
	}
	void Clip::SetParameters(const ClipParameters& _parameters) noexcept
	{
		parameters = _parameters;
	}
	

	RectangleClipParameters::RectangleClipParameters(RectF _rect) noexcept
		: rect(_rect)
	{
	}
	void RectangleClipParameters::AdjustToWindow(PGUI::Core::Window* window)
	{
		rect = window->GetClientRect();
	}

	RoundedRectangleClipParameters::RoundedRectangleClipParameters(RoundedRect _roundedRect, AdjustFlags _flags) noexcept
		: roundedRect(_roundedRect), flags(_flags)
	{
	}
	void RoundedRectangleClipParameters::AdjustToWindow(PGUI::Core::Window* window)
	{
		RectF rect = window->GetClientRect();

		if ((flags & AdjustFlags::AdjustRect) != AdjustFlags::AdjustNone)
		{
			roundedRect.left = rect.left;
			roundedRect.top = rect.top;
			roundedRect.right = rect.right;
			roundedRect.bottom = rect.bottom;
		}
	}

	EllipseClipParameters::EllipseClipParameters(Ellipse _ellipse, AdjustFlags _flags) noexcept
		: ellipse(_ellipse), flags(_flags)
	{
	}

	void EllipseClipParameters::AdjustToWindow(PGUI::Core::Window* window)
	{
		RectF rect = window->GetClientRect();

		if ((flags & AdjustFlags::AdjustCenter) != AdjustFlags::AdjustNone)
		{
			auto center = rect.Center();
			ellipse.center = center;
		}
		if ((flags & AdjustFlags::AdjustRadii) != AdjustFlags::AdjustNone)
		{
			auto size = rect.Size();

			ellipse.xRadius = size.cx / 2;
			ellipse.yRadius = size.cy / 2;
		}
	}
	
	void AdjustClipForWindow(Clip& clip, PGUI::Core::Window* window) noexcept
	{
		auto& params = clip.GetParameters();

		std::visit([window]<typename T>(T& parameter)
		{
			if constexpr (IsClipParametersAdjustableForWindow<T>)
			{
				parameter.AdjustToWindow(window);
			}
		}, params);

		clip.CreateClip();
	}
}
