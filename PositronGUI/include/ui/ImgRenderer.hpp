#pragma once

#include "core/DirectCompositionWindow.hpp"
#include "helpers/EnumFlag.hpp"
#include "ui/bmp/BitmapSource.hpp"
#include "ui/Colors.hpp"
#include "ui/bmp/BitmapDecoder.hpp"
#include "graphics/BitmapRenderTarget.hpp"

#include <variant>


namespace PGUI::UI
{
	enum class FrameDisposal
	{
		UNDEFINED = 0,
		NONE = 1,
		BACKGROUND = 2,
		PREVIOUS = 3
	};
}
EnableEnumFlag(PGUI::UI::FrameDisposal)

namespace PGUI::UI
{
	using BmpToRender = std::variant<Bmp::BitmapSource, Bmp::BitmapDecoder>;

	class IImgRenderer
	{
		public:
		virtual ~IImgRenderer() = default;
		virtual void Render(Core::WindowPtr<Core::DirectCompositionWindow> wnd) = 0;
		[[nodiscard]] virtual auto GetImage() const noexcept -> BmpToRender = 0;
	};

	class BitmapSourceRenderer : public IImgRenderer
	{
		public:
		explicit BitmapSourceRenderer(Bmp::BitmapSource bmpSrc) noexcept;
		void Render(Core::WindowPtr<Core::DirectCompositionWindow> wnd) override;
		[[nodiscard]] auto GetImage() const noexcept -> BmpToRender override;

		private:
		ComPtr<ID2D1Bitmap> bmp = nullptr;
		Bmp::BitmapSource bmpSrc;
	};
	class GifRenderer : public IImgRenderer
	{
		struct FrameData
		{
			RectF framePosition;
			FrameDisposal disposal = FrameDisposal::UNDEFINED;
			std::chrono::milliseconds frameDelay = std::chrono::milliseconds(0);
		};

		public:
		explicit GifRenderer(Core::WindowPtr<Core::DirectCompositionWindow> wnd, Bmp::BitmapDecoder decoder) noexcept;
		void Render(Core::WindowPtr<Core::DirectCompositionWindow> wnd) noexcept override;
		[[nodiscard]] auto GetImage() const noexcept -> BmpToRender override;
		void OnSize(Core::CWindowPtr<Core::DirectCompositionWindow> wnd);

		private:
		Bmp::BitmapDecoder decoder;
		Graphics::GraphicsBitmap savedBitmap;
		Graphics::BitmapRenderTarget composeRenderTarget;

		std::vector<FrameData> frameData{ };

		SizeU gifSize{ };
		SizeU gifPixelSize{ };
		RGBA backgroundColor = Colors::Transparent;

		static const PGUI::Core::TimerId frameTimer = 1;

		UINT loopCount = 0;
		UINT totalLoopCount = 0;
		bool loop = true;

		std::size_t currentFrameIndex = 0;
		std::size_t nextFrameIndex = 1;

		[[nodiscard]] auto IsLastFrame() const noexcept -> bool;
		[[nodiscard]] auto EndOfAnimation() const noexcept -> bool;

		//[[nodiscard]] auto CalculateDrawRect(const Core::WindowPtr<Core::Window> wnd) const -> RectF;

		void ComposeFrame(Core::WindowPtr<Core::DirectCompositionWindow> wnd);
		void DisposeFrame();
		void OverlayFrame();
		void ClearCurrentFrameArea();
		void SaveComposedFrame();
		void RestoreSavedFrame() const;

		void GetGlobalMetadata();
		void GetBackgroundColor();
		void GetFrameData();
	};
}
