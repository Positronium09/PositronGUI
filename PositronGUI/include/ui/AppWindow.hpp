#pragma once

#include "core/DirectCompositionWindow.hpp"


namespace PGUI::UI
{
	class AppWindow : public Core::DirectCompositionWindow
	{
		public:
		AppWindow() noexcept;

		[[nodiscard]] auto IsFullScreen() const noexcept -> bool;
		void EnterFullScreenMode() noexcept;
		void ExitFullScreenMode() noexcept;

		[[nodiscard]] auto GetMinimumSize() const noexcept -> SizeI;
		void SetMinimumSize(SizeI size) noexcept;

		void SetTitle(std::wstring_view title) const noexcept;
		[[nodiscard]] auto GetTitle() const noexcept -> const std::wstring&;

		[[nodiscard]] auto IsMaximizable() const noexcept -> bool;
		void SetMaximizable(bool isMaximizable) const noexcept;

		[[nodiscard]] auto IsMinimizable() const noexcept -> bool;
		void SetMinimizable(bool isMinimizable) const noexcept;
		
		[[nodiscard]] auto IsAlwaysOnTop() const noexcept -> bool;
		void SetAlwaysOnTop(bool isAlwaysOnTop) const noexcept;

		[[nodiscard]] auto IsResizable() const noexcept -> bool;
		void SetResizeable(bool isResizable) const noexcept;

		private:
		std::wstring titleText;
		WINDOWPLACEMENT prevPlacement{ };
		bool isFullScreen = false;
		SizeI minSize = SizeI{ 300, 300 };

		auto OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnSetText(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnGetMinMaxInfo(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
	};
}
