#pragma once

#include "ui/UIComponent.hpp"
#include "ui/Color.hpp"
#include "ui/Colors.hpp"
#include "ui/bmp/BitmapDecoder.hpp"
#include "ui/bmp/Frame.hpp"
#include "helpers/EnumFlag.hpp"

#include <chrono>


namespace PGUI::UI::Controls
{
	// TODO Add aspect ratio options
	class StaticImage : public UIComponent
	{
		class IRenderer
		{
			public:
			virtual ~IRenderer() = default;

			virtual void Render(StaticImage* staticImage) noexcept = 0;
		};
		class GifRenderer : public IRenderer
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
			explicit GifRenderer(StaticImage* staticImage);
			void Render(StaticImage* staticImage) noexcept override;
			void OnSize(const StaticImage* staticImage);

			private:
			ComPtr<ID2D1Bitmap> savedBitmap;
			ComPtr<ID2D1BitmapRenderTarget> composeRenderTarget;

			SizeU gifSize;
			SizeU gifPixelSize;
			RGBA backgroundColor = Colors::Transparent;

			std::vector<FrameData> frameData;

			const PGUI::Core::TimerId frameTimer = 1;

			UINT loopCount;
			UINT totalLoopCount;
			bool loop;

			std::size_t currentFrameIndex = 0;
			std::size_t nextFrameIndex = 1;

			bool IsLastFrame() const noexcept;
			bool EndOfAnimation() const noexcept;

			[[nodiscard]] RectF CalculateDrawRect(const StaticImage* staticImage) const;

			void GetFrameData(const StaticImage* staticImage);

			void ComposeFrame(StaticImage* staticImage);
			void DisposeFrame(const StaticImage* staticImage);
			void OverlayFrame(const StaticImage* staticImage);
			void ClearCurrentFrameArea(const StaticImage* staticImage);
			void SaveComposedFrame(const StaticImage* staticImage);
			void RestoreSavedFrame(const StaticImage* staticImage) const;

			void GetGlobalMetadata(const StaticImage* staticImage);
			void GetBackgroundColor(const StaticImage* staticImage);
		};
		class StaticRenderer : public IRenderer
		{
			public:
			explicit StaticRenderer(const StaticImage* staticImage) noexcept;
			void Render(StaticImage* staticImage) noexcept override;

			private:
			ComPtr<ID2D1Bitmap> frame;
		};

		public:
		explicit StaticImage(Bmp::BitmapSource img) noexcept;
		explicit StaticImage(std::wstring_view filePath) noexcept;

		[[nodiscard]] Bmp::BitmapSource GetImage() const noexcept;
		[[nodiscard]] std::vector<Bmp::Frame> GetFrames() const noexcept;

		private:
		std::unique_ptr<IRenderer> imageRenderer;
		Bmp::BitmapSource img;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
}
