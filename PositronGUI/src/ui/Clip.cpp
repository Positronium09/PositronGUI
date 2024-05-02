#include "ui/Clip.hpp"

#include "core/Window.hpp"
#include "core/Logger.hpp"
#include "factories/Direct2DFactory.hpp"


namespace PGUI::UI
{
	ComPtr<ID2D1Geometry> EmptyClip::GetClipGeometry()
	{
		return nullptr;
	}
	ID2D1Geometry* EmptyClip::GetClipGeometryPtr()
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

		HRESULT hr = factory->CreateRectangleGeometry(rect, GetHeldComPtrAddress()); HR_T(hr);
	}

	ComPtr<ID2D1Geometry> RectangleClip::GetClipGeometry()
	{
		return GetHeldComPtr();
	}
	ID2D1Geometry* RectangleClip::GetClipGeometryPtr()
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

		HRESULT hr = factory->CreateRoundedRectangleGeometry(roundedRect, GetHeldComPtrAddress()); HR_T(hr);
	}

	ComPtr<ID2D1Geometry> RoundedRectangleClip::GetClipGeometry()
	{
		return GetHeldComPtr();
	}
	ID2D1Geometry* RoundedRectangleClip::GetClipGeometryPtr()
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

	ComPtr<ID2D1Geometry> EllipseClip::GetClipGeometry()
	{
		return GetHeldComPtr();
	}
	ID2D1Geometry* EllipseClip::GetClipGeometryPtr()
	{
		return GetHeldPtr();
	}


	PathClip::PathClip(ComPtr<ID2D1PathGeometry1> geometry) noexcept : 
		ComPtrHolder{ geometry }
	{
		
	}
	ComPtr<ID2D1Geometry> PathClip::GetClipGeometry()
	{
		return GetHeldComPtr();
	}
	ID2D1Geometry* PathClip::GetClipGeometryPtr()
	{
		return GetHeldPtr();
	}

	Clip::Clip(const ClipParameters& _parameters) noexcept : 
		parameters(_parameters)
	{
		CreateClip();
	}
	ClipBase* Clip::Get() const noexcept
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
			else if constexpr (std::is_same_v<T, RectangeClipParameters>)
			{
				clip = std::make_unique<RectangleClip>(parameter.rect);
			}
			else if constexpr (std::is_same_v<T, RoundedRectangeClipParameters>)
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
		clip = nullptr;
	}

	ClipParameters Clip::GetParameters() const noexcept
	{
		return parameters;
	}
	ClipParameters& Clip::GetParameters() noexcept
	{
		return parameters;
	}
	void Clip::SetParameters(const ClipParameters& _parameters) noexcept
	{
		parameters = _parameters;
	}
	

	RectangeClipParameters::RectangeClipParameters(RectF _rect) noexcept
		: rect(_rect)
	{
	}
	void RectangeClipParameters::AdjustToWindow(PGUI::Core::Window* window)
	{
		rect = window->GetClientRect();
	}

	RoundedRectangeClipParameters::RoundedRectangeClipParameters(RoundedRect _roundedRect, AdjustFlags _flags) noexcept
		: roundedRect(_roundedRect), flags(_flags)
	{
	}
	void RoundedRectangeClipParameters::AdjustToWindow(PGUI::Core::Window* window)
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
