#pragma once

#include "core/Rect.hpp"
#include "core/RoundedRect.hpp"
#include "core/Ellipse.hpp"
#include "helpers/EnumFlag.hpp"
#include "helpers/ComPtrHolder.hpp"

#include <memory>
#include <variant>
#include <d2d1.h>


namespace PGUI::Core
{
	class Window;
}

namespace PGUI::UI
{
	enum class ClipType
	{
		Unknown,
		Empty,
		Rect,
		RoundedRect,
		Ellipse,
		Path
	};

	enum class AdjustFlags
	{
		AdjustNone = 0,
		AdjustRect = 1,
		AdjustCenter = 2,
		AdjustRadii = 4
	};
}
EnableEnumFlag(PGUI::UI::ClipType)
EnableEnumFlag(PGUI::UI::AdjustFlags)

namespace PGUI::UI
{
	class ClipBase
	{
		public:
		virtual ~ClipBase() noexcept = default;

		[[nodiscard]] virtual auto GetClipGeometry() -> ComPtr<ID2D1Geometry> = 0;
		[[nodiscard]] virtual auto GetClipGeometryPtr() -> ID2D1Geometry* = 0;
	};

	class EmptyClip : public ClipBase
	{
		public:
		EmptyClip() noexcept = default;

		[[nodiscard]] auto GetClipGeometry() -> ComPtr<ID2D1Geometry> override;
		[[nodiscard]] auto GetClipGeometryPtr() -> ID2D1Geometry* override;
	};

	class RectangleClip : public ClipBase, public ComPtrHolder<ID2D1RectangleGeometry>
	{
		public:
		RectangleClip() noexcept = default;
		explicit RectangleClip(ComPtr<ID2D1RectangleGeometry> geometry) noexcept;
		explicit RectangleClip(RectF rect);

		auto GetClipGeometry() -> ComPtr<ID2D1Geometry> override;
		auto GetClipGeometryPtr() -> ID2D1Geometry* override;
	};

	class RoundedRectangleClip : public ClipBase, public ComPtrHolder<ID2D1RoundedRectangleGeometry>
	{
		public:
		RoundedRectangleClip() noexcept = default;
		explicit RoundedRectangleClip(ComPtr<ID2D1RoundedRectangleGeometry> geometry) noexcept;
		explicit RoundedRectangleClip(RoundedRect rect);

		auto GetClipGeometry() -> ComPtr<ID2D1Geometry> override;
		auto GetClipGeometryPtr() -> ID2D1Geometry* override;
	};

	class EllipseClip : public ClipBase, public ComPtrHolder<ID2D1EllipseGeometry>
	{
		public:
		EllipseClip() noexcept = default;
		explicit EllipseClip(ComPtr<ID2D1EllipseGeometry> geometry) noexcept;
		explicit EllipseClip(PGUI::Ellipse ellipse);

		auto GetClipGeometry() -> ComPtr<ID2D1Geometry> override;
		auto GetClipGeometryPtr() -> ID2D1Geometry* override;
	};

	class PathClip : public ClipBase, public ComPtrHolder<ID2D1PathGeometry1>
	{
		PathClip() noexcept = default;
		explicit PathClip(ComPtr<ID2D1PathGeometry1> geometry) noexcept;

		auto GetClipGeometry() -> ComPtr<ID2D1Geometry> override;
		auto GetClipGeometryPtr() -> ID2D1Geometry* override;
	};

	struct EmptyClipParameters
	{

	};

	struct AdjustableToWindow
	{
		public:
		virtual ~AdjustableToWindow() noexcept = default;

		virtual void AdjustToWindow(PGUI::Core::Window* window) = 0;
	};

	struct RectangleClipParameters : public AdjustableToWindow
	{
		RectF rect;

		explicit RectangleClipParameters(RectF rect) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	struct RoundedRectangleClipParameters : public AdjustableToWindow
	{
		RoundedRect roundedRect;
		AdjustFlags flags = AdjustFlags::AdjustNone;

		explicit RoundedRectangleClipParameters(
			RoundedRect roundedRect, AdjustFlags flags = AdjustFlags::AdjustRect) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	struct EllipseClipParameters : public AdjustableToWindow
	{
		Ellipse ellipse;
		AdjustFlags flags = AdjustFlags::AdjustNone;

		explicit EllipseClipParameters(
			Ellipse ellipse, AdjustFlags flags = AdjustFlags(AdjustFlags::AdjustCenter | AdjustFlags::AdjustRadii)) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	using ClipParameters = 
		std::variant<EmptyClipParameters, 
		RectangleClipParameters, 
		RoundedRectangleClipParameters, 
		EllipseClipParameters>;

	template<typename T>
	concept IsEmptyClipParameters = std::is_same_v<T, EmptyClipParameters>;

	template<typename T>
	concept IsClipParametersAdjustableForWindow = std::derived_from<T, AdjustableToWindow>;

	class Clip final
	{
		public:
		explicit(false) Clip(ClipParameters  parameters = EmptyClipParameters{ }) noexcept;

		[[nodiscard]] auto Get() const noexcept -> ClipBase*;

		void CreateClip() noexcept;
		void ReleaseClip() noexcept;

		[[nodiscard]] auto GetParameters() const noexcept -> ClipParameters;
		[[nodiscard]] auto GetParameters() noexcept -> ClipParameters&;
		void SetParameters(const ClipParameters& parameters) noexcept;

		auto operator=(const ClipParameters& _parameters) noexcept -> Clip& { parameters = _parameters; return *this; }
		[[nodiscard]] auto operator->() const noexcept -> ClipBase* { return clip.get(); }
		[[nodiscard]] explicit(false) operator ClipBase* () const noexcept { return clip.get(); }
		[[nodiscard]] explicit operator bool() const noexcept { return (bool)clip; }

		private:
		std::unique_ptr<ClipBase> clip = nullptr;
		ClipParameters parameters;
	};

	void AdjustClipForWindow(Clip& clip, PGUI::Core::Window* window) noexcept;
}
