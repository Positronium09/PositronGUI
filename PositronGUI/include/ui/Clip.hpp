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
	struct _adjust_flags
	{
		enum EnumValues
		{
			Unknown,
			Empty,
			Rect,
			RoundedRect,
			Ellipse,
			Path
		};
	};
	using ClipType = EnumFlag<_adjust_flags>;

	class ClipBase
	{
		public:
		virtual ~ClipBase() noexcept = default;

		virtual [[nodiscard]] ComPtr<ID2D1Geometry> GetClipGeometry() = 0;
		virtual [[nodiscard]] ID2D1Geometry* GetClipGeometryPtr() = 0;
	};

	class EmptyClip : public ClipBase
	{
		public:
		EmptyClip() noexcept = default;

		[[nodiscard]] ComPtr<ID2D1Geometry> GetClipGeometry() override;
		[[nodiscard]] ID2D1Geometry* GetClipGeometryPtr() override;
	};

	class RectangleClip : public ClipBase, public ComPtrHolder<ID2D1RectangleGeometry>
	{
		public:
		RectangleClip() noexcept = default;
		explicit RectangleClip(ComPtr<ID2D1RectangleGeometry> geometry) noexcept;
		explicit RectangleClip(RectF rect);

		ComPtr<ID2D1Geometry> GetClipGeometry() override;
		ID2D1Geometry* GetClipGeometryPtr() override;
	};

	class RoundedRectangleClip : public ClipBase, public ComPtrHolder<ID2D1RoundedRectangleGeometry>
	{
		public:
		RoundedRectangleClip() noexcept = default;
		explicit RoundedRectangleClip(ComPtr<ID2D1RoundedRectangleGeometry> geometry) noexcept;
		explicit RoundedRectangleClip(RoundedRect rect);

		ComPtr<ID2D1Geometry> GetClipGeometry() override;
		ID2D1Geometry* GetClipGeometryPtr() override;
	};

	class EllipseClip : public ClipBase, public ComPtrHolder<ID2D1EllipseGeometry>
	{
		public:
		EllipseClip() noexcept = default;
		explicit EllipseClip(ComPtr<ID2D1EllipseGeometry> geometry) noexcept;
		explicit EllipseClip(PGUI::Ellipse ellipse);

		ComPtr<ID2D1Geometry> GetClipGeometry() override;
		ID2D1Geometry* GetClipGeometryPtr() override;
	};

	class PathClip : public ClipBase, public ComPtrHolder<ID2D1PathGeometry1>
	{
		PathClip() noexcept = default;
		explicit PathClip(ComPtr<ID2D1PathGeometry1> geometry) noexcept;

		ComPtr<ID2D1Geometry> GetClipGeometry() override;
		ID2D1Geometry* GetClipGeometryPtr() override;
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

	struct RectangeClipParameters : public AdjustableToWindow
	{
		RectF rect;

		explicit RectangeClipParameters(RectF rect) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	struct RoundedRectangeClipParameters : public AdjustableToWindow
	{
		struct _adjust_flags
		{
			enum EnumValues
			{
				AdjustNone = 0,
				AdjustRect = 1,
				AdjustRadii = 2
			};
		};
		using AdjustFlags = EnumFlag<_adjust_flags>;

		RoundedRect roundedRect;
		AdjustFlags flags = AdjustFlags::AdjustNone;

		explicit RoundedRectangeClipParameters(
			RoundedRect roundedRect, AdjustFlags flags = AdjustFlags::AdjustRect) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	struct EllipseClipParameters : public AdjustableToWindow
	{
		struct _adjust_flags
		{
			enum EnumValues
			{
				AdjustNone = 0,
				AdjustCenter = 1,
				AdjustRadii = 2
			};
		};
		using AdjustFlags = EnumFlag<_adjust_flags>;

		Ellipse ellipse;
		AdjustFlags flags = AdjustFlags::AdjustNone;

		explicit EllipseClipParameters(
			Ellipse ellipse, AdjustFlags flags = AdjustFlags(AdjustFlags::AdjustCenter | AdjustFlags::AdjustRadii)) noexcept;

		void AdjustToWindow(PGUI::Core::Window* window) override;
	};

	using ClipParameters = 
		std::variant<EmptyClipParameters, 
		RectangeClipParameters, 
		RoundedRectangeClipParameters, 
		EllipseClipParameters>;

	template<typename T>
	concept IsEmptyClipParameters = std::is_same_v<T, EmptyClipParameters>;

	template<typename T>
	concept IsClipParametersAdjustableForWindow = std::derived_from<T, AdjustableToWindow>;

	class Clip final
	{
		public:
		explicit(false) Clip(const ClipParameters& parameters = EmptyClipParameters{ }) noexcept;

		[[nodiscard]] ClipBase* Get() const noexcept;

		void CreateClip() noexcept;
		void ReleaseClip() noexcept;

		[[nodiscard]] ClipParameters GetParameters() const noexcept;
		[[nodiscard]] ClipParameters& GetParameters() noexcept;
		void SetParameters(const ClipParameters& parameters) noexcept;

		void operator=(const ClipParameters& _parameters) noexcept { parameters = _parameters; }
		[[nodiscard]] ClipBase* operator->() const noexcept { return clip.get(); }
		[[nodiscard]] explicit(false) operator ClipBase* () const noexcept { return clip.get(); }
		[[nodiscard]] explicit operator bool() const noexcept { return (bool)clip; }

		private:
		std::unique_ptr<ClipBase> clip = nullptr;
		ClipParameters parameters;
	};

	void AdjustClipForWindow(Clip& clip, PGUI::Core::Window* window) noexcept;
}
