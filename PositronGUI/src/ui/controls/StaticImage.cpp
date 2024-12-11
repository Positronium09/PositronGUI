#include <utility>

#include "ui/controls/StaticImage.hpp"

#include "ui/bmp/Frame.hpp"
#include "ui/Color.hpp"
#include "helpers/HelperFunctions.hpp"
#include "factories/WICFactory.hpp"

 
namespace PGUI::UI::Controls
{
	StaticImage::StaticImage(std::wstring_view fileName) : 
		StaticImage{ Bmp::BitmapDecoder{ fileName } }
	{
	}

	StaticImage::StaticImage(const BmpToRender& bmp) : 
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
		CreateRenderer(std::move(bmp));
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
				const auto& decoder = std::get<Bmp::BitmapDecoder>(bmp);
				auto frameCount = decoder.GetFrameCount();
				if (frameCount == 0)
				{
					throw PGUI::Core::HresultException{ WINCODEC_ERR_FRAMEMISSING };
				}
				if (frameCount == 1)
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

	auto StaticImage::OnCreate(BmpToRender bmp, UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		CreateRenderer(std::move(bmp));

		return 0;
	}

	auto StaticImage::OnPaint(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		BeginDraw();

		renderer->Render(this);

		EndDraw();

		return 0;
	}

	auto StaticImage::OnSize(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const noexcept -> Core::HandlerResult
	{
		if (auto* gifRenderer = dynamic_cast<GifRenderer*>(renderer.get()))
		{
			gifRenderer->OnSize(this);
		}

		return 0;
	}
}
