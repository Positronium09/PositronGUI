#include <utility>

#include "ui/controls/StaticImage.hpp"

#include "ui/Color.hpp"
#include "ui/bmp/MetadataReader.hpp"
#include "ui/bmp/Palette.hpp"

#include "graphics/GraphicsBitmap.hpp"
#include "graphics/BitmapRenderTarget.hpp"
#include "helpers/HelperFunctions.hpp"
#include "factories/WICFactory.hpp"

 
namespace PGUI::UI::Controls
{
	using namespace std::chrono_literals;

	#pragma region BitmapSourceRenderer

	StaticImage::BitmapSourceRenderer::BitmapSourceRenderer(Bmp::BitmapSource bmpSrc) noexcept :
		bmpSrc{std::move( bmpSrc )}
	{
	}
	
	void StaticImage::BitmapSourceRenderer::Render(StaticImage* img)
	{
		auto g = img->GetGraphics();

		if (!bmp)
		{
			bmp = g.CreateBitmap(bmpSrc);
		}

		g.DrawBitmap(Graphics::GraphicsBitmap{ bmp }, img->GetClientRect());
	}

	auto StaticImage::BitmapSourceRenderer::GetImage() const noexcept -> BmpToRender
	{
		return bmpSrc;
	}

	#pragma endregion

	#pragma region GifRenderer

	StaticImage::GifRenderer::GifRenderer(StaticImage* staticImage, Bmp::BitmapDecoder decoder) noexcept :
		decoder{std::move( decoder )}
	{
		composeRenderTarget = staticImage->GetGraphics().CreateCompatibleRenderTarget();

		GetGlobalMetadata();
		GetFrameData();
		ComposeFrame(staticImage);
	}

	void StaticImage::GifRenderer::Render(StaticImage* staticImage) noexcept
	{
		if (!composeRenderTarget)
		{
			return;
		}

		auto frameToRender = composeRenderTarget.GetBitmap();

		auto g = staticImage->GetGraphics();

		auto drawRect = staticImage->GetClientRect();
		//auto drawRect = CalculateDrawRect(staticImage);

		g.Clear(Colors::Transparent);

		g.DrawBitmap(Graphics::GraphicsBitmap{ frameToRender }, drawRect);
	}

	auto StaticImage::GifRenderer::GetImage() const noexcept -> BmpToRender
	{
		return decoder;
	}

	void StaticImage::GifRenderer::OnSize(const StaticImage* staticImage)
	{
		composeRenderTarget = staticImage->GetGraphics().CreateCompatibleRenderTarget(
			gifPixelSize);
	}

	auto StaticImage::GifRenderer::IsLastFrame() const noexcept -> bool
	{
		return nextFrameIndex == 0;
	}
	auto StaticImage::GifRenderer::EndOfAnimation() const noexcept -> bool
	{
		return loop && IsLastFrame() && totalLoopCount == loopCount;
	}

	void StaticImage::GifRenderer::ComposeFrame(StaticImage* staticImage)
	{
		staticImage->RemoveTimer(frameTimer);

		DisposeFrame();
		OverlayFrame();

		while (frameData[currentFrameIndex].frameDelay == 0ms && !IsLastFrame())
		{
			DisposeFrame();
			OverlayFrame();
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

	void StaticImage::GifRenderer::DisposeFrame()
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

	void StaticImage::GifRenderer::OverlayFrame()
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
		auto s = b.GetSize();
		auto ps = b.GetPixelSize();

		composeRenderTarget.DrawBitmap(
			composeRenderTarget.CreateBitmap(decoder.GetFrame(static_cast<UINT>(currentFrameIndex))),
			currentFrameData.framePosition);

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);

		currentFrameIndex = nextFrameIndex;
		nextFrameIndex++;
		nextFrameIndex %= frameData.size();
	}

	void StaticImage::GifRenderer::ClearCurrentFrameArea()
	{
		composeRenderTarget->BeginDraw();

		composeRenderTarget->PushAxisAlignedClip(
			frameData[currentFrameIndex].framePosition,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		composeRenderTarget->Clear(backgroundColor);

		composeRenderTarget->PopAxisAlignedClip();

		HRESULT hr = composeRenderTarget->EndDraw(); HR_L(hr);
	}

	void StaticImage::GifRenderer::SaveComposedFrame()
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

	void StaticImage::GifRenderer::RestoreSavedFrame() const
	{
		if (!savedBitmap)
		{
			return;
		}

		auto frameToCopyTo = composeRenderTarget.GetBitmap();

		frameToCopyTo.CopyFromBitmap(savedBitmap);
	}

	void StaticImage::GifRenderer::GetGlobalMetadata()
	{
		GetBackgroundColor();

		auto metadataReader = decoder.GetMetadata();

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

	void StaticImage::GifRenderer::GetBackgroundColor()
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

	void StaticImage::GifRenderer::GetFrameData()
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

	#pragma region StaticImage

	StaticImage::StaticImage(std::wstring_view fileName) : 
		StaticImage{ Bmp::BitmapDecoder{ fileName } }
	{
	}

	StaticImage::StaticImage(BmpToRender bmp) : 
		UIComponent{ Core::WindowClass::Create(L"StaticImage_UIComponent") }
	{
		RegisterMessageHandler(WM_CREATE, std::bind_front(&StaticImage::OnCreate, this, bmp));
		RegisterMessageHandler(WM_SIZE, &StaticImage::OnSize);
		RegisterMessageHandler(WM_PAINT, &StaticImage::OnPaint);
	}

	auto StaticImage::GetImage() const noexcept -> BmpToRender
	{
		return renderer->GetImage();
	}

	void StaticImage::SetImage(BmpToRender bmp) noexcept
	{
		CreateRenderer(bmp);
		Invalidate();
	}

	void StaticImage::CreateRenderer(BmpToRender bmp) noexcept
	{
		std::visit([this, bmp]<typename T>(T& param)
		{
			if constexpr (std::is_same_v<T, Bmp::BitmapSource>)
			{
				renderer = std::make_unique<BitmapSourceRenderer>(param);
			}
			else if constexpr (std::is_same_v<T, Bmp::BitmapDecoder>)
			{
				auto& decoder = std::get<Bmp::BitmapDecoder>(bmp);
				if (auto frameCount = decoder.GetFrameCount();
					frameCount == 0)
				{
					throw PGUI::Core::HresultException{ WINCODEC_ERR_FRAMEMISSING };
				}
				else if (frameCount == 1)
				{
					renderer = std::make_unique<BitmapSourceRenderer>(decoder.GetFrame(0));
				}
				else
				{
					renderer = std::make_unique<GifRenderer>(this, decoder);
				}
			}
		}, bmp);
	}

	auto StaticImage::OnCreate(BmpToRender bmp, UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		CreateRenderer(bmp);

		return 0;
	}

	auto StaticImage::OnPaint(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		BeginDraw();

		renderer->Render(this);

		EndDraw();

		return 0;
	}

	auto StaticImage::OnSize(UINT, WPARAM, LPARAM) const noexcept -> Core::HandlerResult
	{
		if (auto gifRenderer = dynamic_cast<GifRenderer*>(renderer.get()))
		{
			gifRenderer->OnSize(this);
		}

		return 0;
	}

	#pragma endregion
}
