#pragma once

#include "ui/UIComponent.hpp"
#include "ui/img/Image.hpp"
#include "ui/img/Frame.hpp"


namespace PGUI::UI::Controls
{
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
			public:
			explicit GifRenderer(ComPtr<ID2D1RenderTarget> rt, Img::Image img) noexcept;
			void Render(StaticImage* staticImage) noexcept override;

			private:
			std::size_t currentFrame = 0;
			std::vector<ComPtr<ID2D1Bitmap>> frames;
		};
		class StaticRenderer : public IRenderer
		{
			public:
			explicit StaticRenderer(ComPtr<ID2D1RenderTarget> rt, Img::Image img) noexcept;
			void Render(StaticImage* staticImage) noexcept override;

			private:
			ComPtr<ID2D1Bitmap> frame;
		};

		public:
		explicit StaticImage(Img::Image img) noexcept;
		explicit StaticImage(std::wstring_view filePath) noexcept;

		[[nodiscard]] Img::Image GetImage() const noexcept;
		[[nodiscard]] std::vector<Img::Frame> GetFrames() const noexcept;

		private:
		std::unique_ptr<IRenderer> imageRenderer;
		Img::Image img;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
