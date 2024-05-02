#pragma once

#include "ui/UIComponent.hpp"
#include "ui/img/Image.hpp"
#include "ui/img/Frame.hpp"


namespace PGUI::UI::Controls
{
	class ImageButton : public UIComponent
	{
		public:
		explicit ImageButton(Img::Image img) noexcept;
		explicit ImageButton(std::wstring_view filePath) noexcept;

		[[nodiscard]] Img::Image GetImage() const noexcept;
		[[nodiscard]] const std::vector<Img::Frame>& GetFrames() const noexcept;
		[[nodiscard]] std::vector<Img::Frame>& GetFrames() noexcept;

		private:
		Img::Image img;
		std::vector<Img::Frame> frames;

		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
