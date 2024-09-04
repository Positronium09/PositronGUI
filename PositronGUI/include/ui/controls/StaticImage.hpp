#pragma once

#include "ui/UIComponent.hpp"
#include "ui/Color.hpp"
#include "ui/Colors.hpp"
#include "ui/bmp/BitmapDecoder.hpp"
#include "ui/bmp/Frame.hpp"
#include "helpers/EnumFlag.hpp"
#include "graphics/GraphicsBitmap.hpp"
#include "graphics/BitmapRenderTarget.hpp"

#include <chrono>
#include <variant>


namespace PGUI::UI::Controls
{
	enum class FrameDisposal
	{
		UNDEFINED = 0,
		NONE = 1,
		BACKGROUND = 2,
		PREVIOUS = 3
	};
}
EnableEnumFlag(PGUI::UI::Controls::FrameDisposal)

namespace PGUI::UI::Controls
{
	using BmpToRender = std::variant<Bmp::BitmapSource, Bmp::BitmapDecoder>;
	// TODO Add aspect ratio options
	// TODO Extract Renderers from this class
	class StaticImage : public UIComponent
	{
		class IImgRenderer
		{
			public:
			virtual ~IImgRenderer() = default;
			virtual void Render(StaticImage* img) = 0;
			[[nodiscard]] virtual auto GetImage() const noexcept -> BmpToRender = 0;
		};

		class BitmapSourceRenderer : public IImgRenderer
		{
			public:
			explicit BitmapSourceRenderer(Bmp::BitmapSource bmpSrc) noexcept;
			void Render(StaticImage* img) override;
			[[nodiscard]] auto GetImage() const noexcept -> BmpToRender override;

			private:
			ComPtr<ID2D1Bitmap> bmp;
			Bmp::BitmapSource bmpSrc;
		};
		class GifRenderer : public IImgRenderer
		{
			struct FrameData
			{
				RectF framePosition{ };
				FrameDisposal disposal = FrameDisposal::UNDEFINED;
				std::chrono::milliseconds frameDelay = std::chrono::milliseconds(0);
			};

			public:
			explicit GifRenderer(StaticImage* staticImage, Bmp::BitmapDecoder decoder) noexcept;
			void Render(StaticImage* staticImage) noexcept override;
			[[nodiscard]] auto GetImage() const noexcept -> BmpToRender override;
			void OnSize(const StaticImage* staticImage);

			private:
			Bmp::BitmapDecoder decoder;
			Graphics::GraphicsBitmap savedBitmap;
			Graphics::BitmapRenderTarget composeRenderTarget;

			std::vector<FrameData> frameData;

			SizeU gifSize{ };
			SizeU gifPixelSize{ };
			RGBA backgroundColor = Colors::Transparent;

			const PGUI::Core::TimerId frameTimer = 1;

			UINT loopCount = 0;
			UINT totalLoopCount = 0;
			bool loop = true;

			std::size_t currentFrameIndex = 0;
			std::size_t nextFrameIndex = 1;

			[[nodiscard]] auto IsLastFrame() const noexcept -> bool;
			[[nodiscard]] auto EndOfAnimation() const noexcept -> bool;

			[[nodiscard]] auto CalculateDrawRect(const StaticImage* staticImage) const -> RectF;

			void ComposeFrame(StaticImage* staticImage);
			void DisposeFrame();
			void OverlayFrame();
			void ClearCurrentFrameArea();
			void SaveComposedFrame();
			void RestoreSavedFrame() const;

			void GetGlobalMetadata();
			void GetBackgroundColor();
			void GetFrameData();
		};

		public:
		explicit StaticImage(std::wstring_view fileName);
		explicit StaticImage(BmpToRender bmp);

		[[nodiscard]] auto GetImage() const noexcept -> BmpToRender;
		void SetImage(BmpToRender bmp) noexcept;

		private:
		void CreateRenderer(BmpToRender bmp) noexcept;
		std::unique_ptr<IImgRenderer> renderer;

		auto OnCreate(BmpToRender bmp, UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
