#include "ui/controls/ImageButton.hpp"


namespace PGUI::UI::Controls
{
	ImageButton::ImageButton(Img::Image img) noexcept : 
		img{ img }, frames{ img.GetFrames() }
	{
	}
	ImageButton::ImageButton(std::wstring_view filePath) noexcept : 
		ImageButton{ Img::Image{ filePath } }
	{
	}

	Img::Image ImageButton::GetImage() const noexcept
	{
		return img;
	}
	const std::vector<Img::Frame>& ImageButton::GetFrames() const noexcept
	{
		return frames;
	}
	std::vector<Img::Frame>& ImageButton::GetFrames() noexcept
	{
		return frames;
	}
}
