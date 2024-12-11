#include "ui/ImgRenderer.hpp"

#include "graphics/GraphicsBitmap.hpp"
#include "helpers/PropVariant.hpp"
#include "ui/bmp/MetadataReader.hpp"
#include "ui/bmp/Palette.hpp"

#include <chrono>


namespace PGUI::UI
{
	using namespace std::chrono_literals;

	#pragma region BitmapSourceRenderer

	BitmapSourceRenderer::BitmapSourceRenderer(Bmp::BitmapSource bmpSrc) noexcept :
		bmpSrc{ std::move(bmpSrc) }
	{
	}

	void BitmapSourceRenderer::Render(Core::WindowPtr<Core::DirectCompositionWindow> wnd)
	{
		auto g = wnd->GetGraphics();

		if (!bmp)
		{
			bmp = g.CreateBitmap(bmpSrc);
		}

		g.DrawBitmap(Graphics::GraphicsBitmap{ bmp }, wnd->GetClientRect());
	}

	auto BitmapSourceRenderer::GetImage() const noexcept -> BmpToRender
	{
		return bmpSrc;
	}

	#pragma endregion

	#pragma region GifRenderer

	GifRenderer::GifRenderer(Core::WindowPtr<Core::DirectCompositionWindow> wnd, Bmp::BitmapDecoder decoder) noexcept :
		decoder{ std::move(decoder) },
		composeRenderTarget{ wnd->GetGraphics().CreateCompatibleRenderTarget() }
	{
		GetGlobalMetadata();
		GetFrameData();
		ComposeFrame(wnd);
	}

	void GifRenderer::Render(Core::WindowPtr<Core::DirectCompositionWindow> wnd) noexcept
	{
		if (!composeRenderTarget)
		{
			return;
		}

		auto frameToRender = composeRenderTarget.GetBitmap();

		auto g = wnd->GetGraphics();

		auto drawRect = wnd->GetClientRect();
		//auto drawRect = CalculateDrawRect(wnd);

		g.Clear(Colors::Transparent);

		g.DrawBitmap(Graphics::GraphicsBitmap{ frameToRender }, drawRect);
	}

	auto GifRenderer::GetImage() const noexcept -> BmpToRender
	{
		return decoder;
	}

	void GifRenderer::OnSize(const Core::CWindowPtr<Core::DirectCompositionWindow> wnd)
	{
		composeRenderTarget = wnd->GetGraphics().CreateCompatibleRenderTarget(
			gifPixelSize);
	}

	auto GifRenderer::IsLastFrame() const noexcept -> bool
	{
		return nextFrameIndex == 0;
	}
	auto GifRenderer::EndOfAnimation() const noexcept -> bool
	{
		return loop && IsLastFrame() && totalLoopCount == loopCount;
	}

	void GifRenderer::ComposeFrame(Core::WindowPtr<Core::DirectCompositionWindow> wnd)
	{
		wnd->RemoveTimer(frameTimer);

		DisposeFrame();
		OverlayFrame();

		while (frameData[currentFrameIndex].frameDelay == 0ms && !IsLastFrame())
		{
			DisposeFrame();
			OverlayFrame();
		}

		if (!EndOfAnimation() && frameData.size() > 1)
		{
			wnd->AddTimer(frameTimer,
				frameData[currentFrameIndex].frameDelay,
				[this, wnd](Core::TimerId)
			{
				ComposeFrame(wnd);
			});
		}
		wnd->Invalidate();
	}

	void GifRenderer::DisposeFrame()
	{
		auto dispose = frameData[currentFrameIndex].disposal;

		switch (dispose)
		{
			case FrameDisposal::BACKGROUND:
				ClearCurrentFrameArea();
				return;
			case FrameDisposal::PREVIOUS:
				RestoreSavedFrame();
				return;
			default:
				return;
		}
	}

	void GifRenderer::OverlayFrame()
	{
		const auto& currentFrameData = frameData[currentFrameIndex];

		if (currentFrameData.disposal == FrameDisposal::PREVIOUS)
		{
			SaveComposedFrame();
		}

		composeRenderTarget->BeginDraw();

		if (currentFrameIndex == 0)
		{
			composeRenderTarget->Clear(backgroundColor);
			loopCount++;
		}

		auto b = composeRenderTarget.GetBitmap();

		composeRenderTarget.DrawBitmap(
			composeRenderTarget.CreateBitmap(decoder.GetFrame(static_cast<UINT>(currentFrameIndex))),
			currentFrameData.framePosition);

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);

		currentFrameIndex = nextFrameIndex;
		nextFrameIndex++;
		nextFrameIndex %= frameData.size();
	}

	void GifRenderer::ClearCurrentFrameArea()
	{
		composeRenderTarget->BeginDraw();

		composeRenderTarget->PushAxisAlignedClip(
			frameData[currentFrameIndex].framePosition,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		composeRenderTarget->Clear(backgroundColor);

		composeRenderTarget->PopAxisAlignedClip();

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);
	}

	void GifRenderer::SaveComposedFrame()
	{
		auto toSave = composeRenderTarget.GetBitmap();

		SizeU bitmapSize = toSave->GetPixelSize();
		D2D1_BITMAP_PROPERTIES bitmapProp{ };
		toSave->GetDpi(&bitmapProp.dpiX, &bitmapProp.dpiY);
		bitmapProp.pixelFormat = toSave->GetPixelFormat();

		savedBitmap = composeRenderTarget.CreateBitmap(
			bitmapSize,
			bitmapProp);

		savedBitmap.CopyFromBitmap(toSave);

	}

	void GifRenderer::RestoreSavedFrame() const
	{
		if (!savedBitmap)
		{
			return;
		}

		auto frameToCopyTo = composeRenderTarget.GetBitmap();

		frameToCopyTo.CopyFromBitmap(savedBitmap);
	}

	void GifRenderer::GetGlobalMetadata()
	{
		GetBackgroundColor();

		auto metadataReader = decoder.GetMetadata();

		auto val = metadataReader[L"/logscrdesc/Width"].GetValue();
		if (const auto* ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			gifSize.cx = *ptr;
		}

		val = metadataReader[L"/logscrdesc/Height"].GetValue();
		if (const auto* ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			gifSize.cy = *ptr;
		}


		gifPixelSize = gifSize;
		val = metadataReader[L"/logscrdesc/PixelAspectRatio"].GetValue();
		if (const auto* ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			auto aspectRatio = *ptr;

			if (aspectRatio != 0)
			{
				auto pixelAspRatio = (aspectRatio + 15.F) / 64.F;

				if (pixelAspRatio > 1.0F)
				{
					gifPixelSize.cy = static_cast<UINT>(static_cast<float>(gifSize.cy) / pixelAspRatio);
				}
				else
				{
					gifPixelSize.cx = static_cast<UINT>(static_cast<float>(gifSize.cx) * pixelAspRatio);
				}
			}
		}


		val = metadataReader[L"/appext/application"];
		if (auto* ptr = std::get_if<std::vector<UCHAR>>(&val);
			ptr != nullptr)
		{
			auto& vec = *ptr;
			if (std::string applicationText{ vec.begin(), vec.end() };
				applicationText != "NETSCAPE2.0" && applicationText != "ANIMEXTS1.0")
			{
				return;
			}

			val = metadataReader[L"/appext/data"];
			if (ptr = std::get_if<std::vector<UCHAR>>(&val);
				ptr != nullptr)
			{
				const auto& loopData = *ptr;

				if (loopData.size() >= 4 &&
					loopData.at(0) > 0 &&
					loopData.at(1) == 1)
				{
					totalLoopCount = static_cast<UINT>(
						static_cast<std::byte>(loopData.at(2)) |
						static_cast<std::byte>(loopData.at(3)) << 8);

					loop = true ? totalLoopCount != 0 : false;
				}

			}
		}
	}

	void GifRenderer::GetBackgroundColor()
	{
		try
		{
			auto metadataReader = decoder.GetMetadata();

			auto val = metadataReader[L"/logscrdesc/GlobalColorTableFlag"];
			if (val.Type() != PropVariantType::Bool || !std::get<bool>(val.GetValue()))
			{
				return;
			}

			val = metadataReader[L"/logscrdesc/BackgroundColorIndex"];
			UCHAR backgroundColorIndex = std::get<UCHAR>(val.GetValue());

			Bmp::Palette palette{ decoder };

			WICColor colorRaw = palette.GetColors()[backgroundColorIndex];

			std::uint8_t blue = colorRaw & 0xff;
			std::uint8_t green = (colorRaw >> 8) & 0xff;
			std::uint8_t red = (colorRaw >> 16) & 0xff;
			std::uint8_t alpha = (colorRaw >> 24) & 0xff;

			backgroundColor = RGBA{ red, green, blue, alpha };
		}
		catch (Core::HresultException& exception)
		{
			HR_L(exception);
		}
	}

	void GifRenderer::GetFrameData()
	{
		auto frames = decoder.GetFrames();
		for (const auto& frame : frames)
		{
			try
			{
				auto metadataReader = frame.GetMetadata();
				FrameData data;

				data.framePosition.left =
					static_cast<float>(std::get<USHORT>(
						metadataReader.GetMetadata(L"/imgdesc/Left").GetValue()));

				data.framePosition.top =
					static_cast<float>(std::get<USHORT>(
						metadataReader.GetMetadata(L"/imgdesc/Top").GetValue()));

				data.framePosition.right = data.framePosition.left +
					static_cast<float>(std::get<USHORT>(
						metadataReader.GetMetadata(L"/imgdesc/Width").GetValue()));

				data.framePosition.bottom = data.framePosition.top +
					static_cast<float>(std::get<USHORT>(
						metadataReader.GetMetadata(L"/imgdesc/Height").GetValue()));

				data.frameDelay = std::get<USHORT>(
					metadataReader.GetMetadata(L"/grctlext/Delay").GetValue()) * 10ms;

				data.disposal = FrameDisposal{ std::get<UCHAR>(
					metadataReader.GetMetadata(L"/grctlext/Disposal").GetValue())
				};

				frameData.push_back(data);
			}
			catch (Core::HresultException& exception)
			{
				HR_L(exception);
			}
		}
	}

	#pragma endregion
}
