#include "ui/controls/StaticImage.hpp"

#include "ui/Color.hpp"


namespace PGUI::UI::Controls
{
	StaticImage::StaticImage(Img::Image img) noexcept : 
		img{ img }
	{
		RegisterMessageHandler(WM_CREATE, &StaticImage::OnCreate);
		RegisterMessageHandler(WM_PAINT, &StaticImage::OnPaint);
	}
	StaticImage::StaticImage(std::wstring_view filePath) noexcept : 
		StaticImage{ Img::Image{ filePath } }
	{
	}

	Img::Image StaticImage::GetImage() const noexcept
	{
		return img;
	}
	std::vector<Img::Frame> StaticImage::GetFrames() const noexcept
	{
		return img.GetFrames();
	}

	Core::HandlerResult StaticImage::OnCreate(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam) noexcept
	{
		if (img.GetFrameCount() > 1)
		{
			imageRenderer = std::make_unique<GifRenderer>(GetRenderingInterface(), img);
		}
		else
		{
			imageRenderer = std::make_unique<StaticRenderer>(GetRenderingInterface(), img);
		}

		return 0;
	}

	Core::HandlerResult StaticImage::OnPaint(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		renderer->Clear(RGBA{ });

		imageRenderer->Render(this);

		EndDraw();

		return 0;
	}
	StaticImage::GifRenderer::GifRenderer(ComPtr<ID2D1RenderTarget> rt, Img::Image img) noexcept
	{
		for (const auto& frame : img.GetFrames())
		{
			frames.push_back(frame.ConvertToD2D1Bitmap(rt));
		}
	}
	void StaticImage::GifRenderer::Render(StaticImage* staticImage) noexcept
	{
		auto renderer = staticImage->GetRenderingInterface();

		renderer->DrawBitmap(frames[currentFrame].Get(), staticImage->GetClientRect());
	}
	StaticImage::StaticRenderer::StaticRenderer(ComPtr<ID2D1RenderTarget> rt, Img::Image img) noexcept : 
		frame{ img.GetFrame().ConvertToD2D1Bitmap(rt) }
	{
	}
	void StaticImage::StaticRenderer::Render(StaticImage* staticImage) noexcept
	{
		auto renderer = staticImage->GetRenderingInterface();

		renderer->DrawBitmap(frame.Get(), staticImage->GetClientRect());
	}
}
