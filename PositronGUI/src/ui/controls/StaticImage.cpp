#include "ui/controls/StaticImage.hpp"

#include "ui/Color.hpp"
#include "ui/bmp/MetadataReader.hpp"
#include "ui/bmp/Palette.hpp"

#include "core/Logger.hpp"
#include "factories/WICFactory.hpp"


namespace PGUI::UI::Controls
{
	using namespace std::chrono_literals;

	StaticImage::StaticImage(Bmp::BitmapSource img) noexcept : 
		img{ img }
	{
		RegisterMessageHandler(WM_CREATE, &StaticImage::OnCreate);
		RegisterMessageHandler(WM_PAINT, &StaticImage::OnPaint);
		RegisterMessageHandler(WM_SIZE, &StaticImage::OnSize);
	}
	StaticImage::StaticImage(std::wstring_view filePath) noexcept : 
		StaticImage{ Bmp::BitmapDecoder{ filePath } }
	{
	}

	Bmp::BitmapSource StaticImage::GetImage() const noexcept
	{
		return img;
	}
	std::vector<Bmp::Frame> StaticImage::GetFrames() const noexcept
	{
		return img.GetFrames();
	}

	Core::HandlerResult StaticImage::OnCreate(UINT, WPARAM, LPARAM) noexcept
	{
		if (auto bmpDecode = dynamic_cast<Bmp::BitmapDecoder*>(&img))
		{

		}
		if (img.GetFrameCount() > 1)
		{
			imageRenderer = std::make_unique<GifRenderer>(this);
		}
		else
		{
			imageRenderer = std::make_unique<StaticRenderer>(this);
		}

		return 0;
	}

	Core::HandlerResult StaticImage::OnPaint(UINT, WPARAM, LPARAM) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		imageRenderer->Render(this);

		EndDraw();

		return 0;
	}

	Core::HandlerResult StaticImage::OnSize(UINT, WPARAM, LPARAM) const noexcept
	{
		if (auto gifRenderer = dynamic_cast<GifRenderer*>(imageRenderer.get());
			gifRenderer != nullptr)
		{
			gifRenderer->OnSize(this);
		}

		return 0;
	}

	
	StaticImage::StaticRenderer::StaticRenderer(const StaticImage* staticImage) noexcept :
		frame{ staticImage->img.GetFrame().ConvertToD2D1Bitmap(staticImage->GetRenderingInterface()) }
	{
	}
	void StaticImage::StaticRenderer::Render(StaticImage* staticImage) noexcept
	{
		auto renderer = staticImage->GetRenderingInterface();

		renderer->Clear(RGBA{ });

		renderer->DrawBitmap(frame.Get(), staticImage->GetClientRect());
	}

	StaticImage::GifRenderer::GifRenderer(StaticImage* staticImage)
	{
		HRESULT hr = staticImage->GetRenderingInterface()->
			CreateCompatibleRenderTarget(composeRenderTarget.GetAddressOf());
		HR_T(hr);

		GetGlobalMetadata(staticImage);
		GetFrameData(staticImage);
		ComposeFrame(staticImage);
	}
	
	void StaticImage::GifRenderer::OnSize(const StaticImage* staticImage)
	{
		composeRenderTarget = nullptr;
		HRESULT hr = staticImage->GetRenderingInterface()->CreateCompatibleRenderTarget(
			gifPixelSize,
			composeRenderTarget.GetAddressOf());
		HR_T(hr);
	}

	void StaticImage::GifRenderer::Render(StaticImage* staticImage) noexcept
	{
		if (!composeRenderTarget)
		{
			return;
		}

		ComPtr<ID2D1Bitmap> frameToRender;
		HRESULT hr = composeRenderTarget->GetBitmap(frameToRender.GetAddressOf()); HR_L(hr);
		
		if (FAILED(hr))
		{
			return;
		}

		auto renderer = staticImage->GetRenderingInterface();

		auto drawRect = staticImage->GetClientRect();
		//auto drawRect = CalculateDrawRect(staticImage);

		renderer->Clear(Colors::Transparent);

		renderer->DrawBitmap(frameToRender.Get(), drawRect);
	}
	
	bool StaticImage::GifRenderer::IsLastFrame() const noexcept
	{
		return nextFrameIndex == 0;
	}

	bool StaticImage::GifRenderer::EndOfAnimation() const noexcept
	{
		return loop && IsLastFrame() && totalLoopCount == loopCount;
	}

	RectF StaticImage::GifRenderer::CalculateDrawRect(const StaticImage* staticImage) const
	{
		RectF drawRect{ };

		auto clientRect = staticImage->GetClientRect();
		drawRect.left = (static_cast<FLOAT>(clientRect.right) - static_cast<FLOAT>(gifPixelSize.cx));
		drawRect.top = (static_cast<FLOAT>(clientRect.bottom) - static_cast<FLOAT>(gifPixelSize.cy));
		drawRect.right = drawRect.left + static_cast<FLOAT>(gifPixelSize.cx);
		drawRect.bottom = drawRect.top + static_cast<FLOAT>(gifPixelSize.cy);

		FLOAT aspectRatio = static_cast<FLOAT>(gifPixelSize.cx) /
			static_cast<FLOAT>(gifPixelSize.cy);

		if (drawRect.left < 0)
		{
			auto newWidth = static_cast<FLOAT>(clientRect.right);
			auto newHeight = newWidth / aspectRatio;
			drawRect.left = 0;
			drawRect.top = (static_cast<FLOAT>(clientRect.bottom) - newHeight) / 2.f;
			drawRect.right = newWidth;
			drawRect.bottom = drawRect.top + newHeight;
		}

		if (drawRect.top < 0)
		{
			auto newHeight = static_cast<FLOAT>(clientRect.bottom);
			auto newWidth = newHeight * aspectRatio;
			drawRect.left = (static_cast<FLOAT>(clientRect.right) - newWidth) / 2.f;
			drawRect.top = 0;
			drawRect.right = drawRect.left + newWidth;
			drawRect.bottom = newHeight;
		}

		return drawRect;
	}

	void StaticImage::GifRenderer::GetFrameData(const StaticImage* staticImage)
	{
		auto frames = staticImage->img.GetFrames();
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
			catch (Core::ErrorHandling::HresultException& exception)
			{
				HR_L(exception);
			}
		}
	}

	void StaticImage::GifRenderer::ComposeFrame(StaticImage* staticImage)
	{
		staticImage->RemoveTimer(frameTimer);

		DisposeFrame(staticImage);
		OverlayFrame(staticImage);

		while (frameData[currentFrameIndex].frameDelay == 0ms && !IsLastFrame())
		{
			DisposeFrame(staticImage);
			OverlayFrame(staticImage);
		}

		if (!EndOfAnimation() && frameData.size() > 1)
		{
			staticImage->AddTimer(frameTimer,
				frameData[currentFrameIndex].frameDelay,
				[this, staticImage](Core::TimerId)
			{
				ComposeFrame(staticImage);
			});
		}
		staticImage->Invalidate();
	}
	
	void StaticImage::GifRenderer::DisposeFrame(const StaticImage* staticImage)
	{
		auto dispose = frameData[currentFrameIndex].disposal;

		switch (dispose)
		{
			case FrameDisposal::BACKGROUND:
				ClearCurrentFrameArea(staticImage);
				return;
			case FrameDisposal::PREVIOUS:
				RestoreSavedFrame(staticImage);
				return;
			default:
				return;
		}
	}
	
	void StaticImage::GifRenderer::OverlayFrame(const StaticImage* staticImage)
	{
		const auto& currentFrameData = frameData[currentFrameIndex];

		if (currentFrameData.disposal == FrameDisposal::PREVIOUS)
		{
			SaveComposedFrame(staticImage);
		}

		composeRenderTarget->BeginDraw();

		if (currentFrameIndex == 0)
		{
			composeRenderTarget->Clear(backgroundColor);
			loopCount++;
		}

		ComPtr<ID2D1Bitmap> b;
		composeRenderTarget->GetBitmap(b.GetAddressOf());
		SizeU s = b->GetSize();
		SizeU ps = b->GetPixelSize();

		composeRenderTarget->DrawBitmap(
			staticImage->img.GetFrame(static_cast<UINT>(currentFrameIndex)).ConvertToD2D1Bitmap(composeRenderTarget).Get(),
			currentFrameData.framePosition);

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);

		currentFrameIndex = nextFrameIndex;
		nextFrameIndex++;
		nextFrameIndex %= frameData.size();
	}

	void StaticImage::GifRenderer::ClearCurrentFrameArea(const StaticImage*)
	{
		composeRenderTarget->BeginDraw();

		composeRenderTarget->PushAxisAlignedClip(
			frameData[currentFrameIndex].framePosition,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		composeRenderTarget->Clear(backgroundColor);

		composeRenderTarget->PopAxisAlignedClip();

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);
	}

	void StaticImage::GifRenderer::SaveComposedFrame(const StaticImage*)
	{
		ComPtr<ID2D1Bitmap> toSave;

		HRESULT hr = composeRenderTarget->GetBitmap(toSave.GetAddressOf()); HR_T(hr);

		SizeU bitmapSize = toSave->GetPixelSize();
		D2D1_BITMAP_PROPERTIES bitmapProp;
		toSave->GetDpi(&bitmapProp.dpiX, &bitmapProp.dpiY);
		bitmapProp.pixelFormat = toSave->GetPixelFormat();

		hr = composeRenderTarget->CreateBitmap(
			bitmapSize,
			bitmapProp,
			&savedBitmap); HR_T(hr);

		savedBitmap->CopyFromBitmap(nullptr, toSave.Get(), nullptr);

	}

	void StaticImage::GifRenderer::RestoreSavedFrame(const StaticImage*) const
	{
		ComPtr<ID2D1Bitmap> frameToCopyTo;

		if (!savedBitmap)
		{
			return;
		}

		HRESULT hr = composeRenderTarget->GetBitmap(frameToCopyTo.GetAddressOf()); HR_L(hr);

		if (SUCCEEDED(hr))
		{
			hr = frameToCopyTo->CopyFromBitmap(nullptr, savedBitmap.Get(), nullptr); HR_L(hr);
		}
	}

	void StaticImage::GifRenderer::GetGlobalMetadata(const StaticImage* staticImage)
	{
		GetBackgroundColor(staticImage);

		auto metadataReader = staticImage->img.GetMetadata();

		auto val = metadataReader[L"/logscrdesc/Width"].GetValue();
		if (auto ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			gifSize.cx = *ptr;
		}
		
		val = metadataReader[L"/logscrdesc/Height"].GetValue();
		if (auto ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			gifSize.cy = *ptr;
		}


		gifPixelSize = gifSize;
		val = metadataReader[L"/logscrdesc/PixelAspectRatio"].GetValue();
		if (auto ptr = std::get_if<USHORT>(&val);
			ptr != nullptr)
		{
			auto aspectRatio = *ptr;
			
			if (aspectRatio != 0)
			{
				auto pixelAspRatio = (aspectRatio + 15.f) / 64.f;

				if (pixelAspRatio > 1.0f)
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
		if (auto ptr = std::get_if<std::vector<UCHAR>>(&val);
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

	void StaticImage::GifRenderer::GetBackgroundColor(const StaticImage* staticImage)
	{
		try
		{
			auto metadataReader = staticImage->img.GetMetadata();
	
			auto val = metadataReader[L"/logscrdesc/GlobalColorTableFlag"];
			if (val.Type() != PropVariantType::Bool || !std::get<bool>(val.GetValue()))
			{
				return;
			}

			val = metadataReader[L"/logscrdesc/BackgroundColorIndex"];
			UCHAR backgroundColorIndex = std::get<UCHAR>(val.GetValue());

			Bmp::Palette palette{ staticImage->img };

			WICColor colorRaw = palette.GetColors()[backgroundColorIndex];

			std::uint8_t blue = colorRaw & 0xff;
			std::uint8_t green = (colorRaw >> 8) & 0xff;
			std::uint8_t red = (colorRaw >> 16) & 0xff;
			std::uint8_t alpha = (colorRaw >> 24) & 0xff;
			
			backgroundColor = RGBA{ red, green, blue, alpha };
		}
		catch (Core::ErrorHandling::HresultException& exception)
		{
			HR_L(exception);
		}
	}
}
