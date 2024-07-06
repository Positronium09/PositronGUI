#pragma once

#include "ui/UIComponent.hpp"
#include "ui/Color.hpp"
#include "ui/Colors.hpp"
#include "ui/bmp/BitmapDecoder.hpp"
#include "ui/bmp/Frame.hpp"
#include "helpers/EnumFlag.hpp"

#include <chrono>
#include <variant>


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
			virtual BmpToRender GetImage() const noexcept = 0;
		};

		class BitmapSourceRenderer : public IImgRenderer
		{
			public:
			explicit BitmapSourceRenderer(Bmp::BitmapSource bmpSrc) noexcept;
			void Render(StaticImage* img) override;
			BmpToRender GetImage() const noexcept override;

			private:
			ComPtr<ID2D1Bitmap> bmp;
			Bmp::BitmapSource bmpSrc;
		};
		class GifRenderer : public IImgRenderer
		{
			struct _frame_disposal_values
			{
				enum EnumValues
				{
					UNDEFINED = 0,
					NONE = 1,
					BACKGROUND = 2,
					PREVIOUS = 3
				};
			};
			using FrameDisposal = EnumFlag<_frame_disposal_values>;

			struct FrameData
			{
				RectF framePosition{ };
				FrameDisposal disposal = FrameDisposal::UNDEFINED;
				std::chrono::milliseconds frameDelay = std::chrono::milliseconds(0);
			};

			public:
			explicit GifRenderer(StaticImage* staticImage, Bmp::BitmapDecoder decoder) noexcept;
			void Render(StaticImage* staticImage) noexcept override;
			BmpToRender GetImage() const noexcept override;
			void OnSize(const StaticImage* staticImage);

			private:
			Bmp::BitmapDecoder decoder;
			ComPtr<ID2D1Bitmap> savedBitmap;
			ComPtr<ID2D1BitmapRenderTarget> composeRenderTarget;

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

			bool IsLastFrame() const noexcept;
			bool EndOfAnimation() const noexcept;

			[[nodiscard]] RectF CalculateDrawRect(const StaticImage* staticImage) const;

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

		BmpToRender GetImage() const noexcept;
		void SetImage(BmpToRender bmp) noexcept;

		private:
		void CreateRenderer(BmpToRender bmp) noexcept;
		std::unique_ptr<IImgRenderer> renderer;

		Core::HandlerResult OnCreate(BmpToRender bmp, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
}
