#include "ui/controls/ImageButton.hpp"


namespace PGUI::UI::Controls
{
	ImageButton::ImageButton(Bmp::BitmapDecoder img) noexcept : 
		img{ img }, frames{ img.GetFrames() }
	{
	}
	ImageButton::ImageButton(std::wstring_view filePath) noexcept : 
		ImageButton{ Bmp::BitmapDecoder{ filePath } }
	{
	}

	Bmp::BitmapDecoder ImageButton::GetImage() const noexcept
	{
		return img;
	}
	const std::vector<Bmp::Frame>& ImageButton::GetFrames() const noexcept
	{
		return frames;
	}
	std::vector<Bmp::Frame>& ImageButton::GetFrames() noexcept
	{
		return frames;
	}
}
