#pragma once

#include "ui/UIComponent.hpp"
#include "ui/bmp/BitmapDecoder.hpp"
#include "ui/bmp/Frame.hpp"


namespace PGUI::UI::Controls
{
	class ImageButton : public UIComponent
	{
		public:
		explicit ImageButton(Bmp::BitmapDecoder img) noexcept;
		explicit ImageButton(std::wstring_view filePath) noexcept;

		[[nodiscard]] Bmp::BitmapDecoder GetImage() const noexcept;
		[[nodiscard]] const std::vector<Bmp::Frame>& GetFrames() const noexcept;
		[[nodiscard]] std::vector<Bmp::Frame>& GetFrames() noexcept;

		private:
		Bmp::BitmapDecoder img;
		std::vector<Bmp::Frame> frames;

		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
