#pragma once

#include "ui/UIComponent.hpp"
#include "ui/ImgRenderer.hpp"

#include <chrono>
#include <variant>


namespace PGUI::UI::Controls
{
	// TODO Add aspect ratio options
	class StaticImage : public UIComponent
	{
		public:
		explicit StaticImage(std::wstring_view fileName);
		explicit StaticImage(const BmpToRender& bmp);

		[[nodiscard]] auto GetImage() const noexcept -> BmpToRender;
		void SetImage(BmpToRender bmp) noexcept;

		private:
		void CreateRenderer(BmpToRender bmp) noexcept;
		std::unique_ptr<IImgRenderer> renderer = nullptr;

		auto OnCreate(BmpToRender bmp, UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
