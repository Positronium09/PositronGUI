#pragma once

#include "core/Rect.hpp"
#include "core/RoundedRect.hpp"
#include "helpers/ComPtr.hpp"

#include <d2d1.h>


namespace PGUI::UI
{
	enum class ClipType
	{
		Empty,
		Unknown,
		Rect,
		RoundedRect,
		Ellipse,
		Path
	};

	class UIComponent;

	class Clip
	{
		public:
		Clip() = default;
		~Clip() = default;

		[[nodiscard]] ClipType GetClipType() const;
		[[nodiscard]] ComPtr<ID2D1Geometry> GetClipGeometry() const;

		static [[nodiscard]] Clip CreateClipFromGeometry(ComPtr<ID2D1Geometry> geometry, ClipType type=ClipType::Unknown);
		static [[nodiscard]] Clip CreateEmptyClip();
		static [[nodiscard]] Clip CreateRectClip(const UIComponent* uiComponent);
		static [[nodiscard]] Clip CreateRoundedRectClip(const UIComponent* uiComponent, float xRadius, float yRadius);
		static [[nodiscard]] Clip CreateEllipseClip(const UIComponent* uiComponent, float xRadius, float yRadius);

		private:
		Clip(ComPtr<ID2D1Geometry> geometry, ClipType type);
		ComPtr<ID2D1Geometry> geometry = nullptr;
		ClipType type = ClipType::Empty;
	};
}
