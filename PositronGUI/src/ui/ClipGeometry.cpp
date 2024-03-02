#include "ui/ClipGeometry.hpp"

#include "ui/UIComponent.hpp"
#include "core/Logger.hpp"
#include "factories/Direct2DFactory.hpp"


namespace PGUI::UI
{
	ClipType Clip::GetClipType() const
	{
		return type;
	}
	ComPtr<ID2D1Geometry> Clip::GetClipGeometry() const
	{
		return geometry;
	}

	Clip Clip::CreateClipFromGeometry(ComPtr<ID2D1Geometry> geometry, ClipType type)
	{
		return Clip{ geometry, type };
	}

	Clip Clip::CreateEmptyClip()
	{
		return CreateClipFromGeometry(nullptr, ClipType::Empty);
	}

	Clip Clip::CreateRectClip(const UIComponent* uiComponent)
	{
		ComPtr<ID2D1RectangleGeometry> geometry;

		HRESULT hr = Direct2DFactory::GetFactory()->CreateRectangleGeometry(
			uiComponent->GetClientRect(), geometry.GetAddressOf()
		); HR_L(hr);

		if (!geometry)
		{
			return CreateEmptyClip();
		}

		return CreateClipFromGeometry(geometry, ClipType::Rect);
	}

	Clip Clip::CreateRoundedRectClip(const UIComponent* uiComponent, float xRadius, float yRadius)
	{
		ComPtr<ID2D1RoundedRectangleGeometry> geometry;

		HRESULT hr = Direct2DFactory::GetFactory()->CreateRoundedRectangleGeometry(
			RoundedRect{ uiComponent->GetClientRect(), xRadius, yRadius }, 
			geometry.GetAddressOf()
		); HR_L(hr);

		if (!geometry)
		{
			return CreateEmptyClip();
		}

		return CreateClipFromGeometry(geometry, ClipType::RoundedRect);
	}

	Clip Clip::CreateEllipseClip(const UIComponent* uiComponent, float xRadius, float yRadius)
	{
		ComPtr<ID2D1EllipseGeometry> geometry;

		HRESULT hr = Direct2DFactory::GetFactory()->CreateEllipseGeometry(
			D2D1::Ellipse(uiComponent->GetClientRect().Center(),
				xRadius, yRadius), 
			geometry.GetAddressOf()
		); HR_L(hr);

		if (!geometry)
		{
			return CreateEmptyClip();
		}

		return CreateClipFromGeometry(geometry, ClipType::Ellipse);
	}

	Clip::Clip(ComPtr<ID2D1Geometry> _geometry, ClipType _type) : 
		geometry(_geometry), type(_type)
	{ }
}
