#pragma once

#include "core/DirectCompositionWindow.hpp"


namespace PGUI::UI
{
	class AppWindow : public Core::DirectCompositionWindow
	{

		public:
		AppWindow();

		bool IsFullScreen() const;
		void EnterFullScreenMode();
		void ExitFullScreenMode();
		void SetMinimumSize(SizeI size);

		void Title(std::wstring_view title) const;
		const std::wstring& Title() const;

		private:
		std::wstring titleText;
		WINDOWPLACEMENT prevPlacement{ };
		bool isFullScreen = false;
		SizeI minSize = SizeI{ 300, 300 };

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSetText(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnGetText(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnGetTextLength(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnGetMinMaxInfo(UINT msg, WPARAM wParam, LPARAM lParam) const;
	};
}
