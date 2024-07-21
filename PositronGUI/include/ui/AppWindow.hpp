#pragma once

#include "core/DirectCompositionWindow.hpp"


namespace PGUI::UI
{
	class AppWindow : public Core::DirectCompositionWindow
	{
		public:
		AppWindow() noexcept;

		[[nodiscard]] bool IsFullScreen() const noexcept;
		void EnterFullScreenMode() noexcept;
		void ExitFullScreenMode() noexcept;

		[[nodiscard]] SizeI GetMinimumSize() const noexcept;
		void SetMinimumSize(SizeI size) noexcept;

		void SetTitle(std::wstring_view title) const noexcept;
		[[nodiscard]] const std::wstring& GetTitle() const noexcept;

		private:
		std::wstring titleText;
		WINDOWPLACEMENT prevPlacement{ };
		bool isFullScreen = false;
		SizeI minSize = SizeI{ 300, 300 };

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnGetMinMaxInfo(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
}
