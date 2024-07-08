#include "ui/AppWindow.hpp"

#include "core/Exceptions.hpp"

#undef min


namespace PGUI::UI
{
	AppWindow::AppWindow() noexcept :
		DirectCompositionWindow{ Core::WindowClass::Create(L"AppWindow") }
	{
		RegisterMessageHandler(WM_NCCREATE, &AppWindow::OnNCCreate);
		RegisterMessageHandler(WM_SETTEXT, &AppWindow::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &AppWindow::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &AppWindow::OnGetTextLength);
		RegisterMessageHandler(WM_GETMINMAXINFO, &AppWindow::OnGetMinMaxInfo);
	}

	bool AppWindow::IsFullScreen() const noexcept
	{
		return isFullScreen;
	}
	void AppWindow::EnterFullScreenMode() noexcept
	{
		if (isFullScreen)
		{
			return;
		}

		GetWindowPlacement(Hwnd(), &prevPlacement);

		DWORD windowStyle = GetWindowLong(Hwnd(), GWL_STYLE);

		MONITORINFO monitorInfo{ };
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfoW(MonitorFromWindow(Hwnd(), MONITOR_DEFAULTTOPRIMARY), &monitorInfo);

		SetWindowLongW(Hwnd(), GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(Hwnd(), HWND_TOP,
			monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		isFullScreen = true;
	}
	void AppWindow::ExitFullScreenMode() noexcept
	{
		if (!isFullScreen)
		{
			return;
		}

		DWORD windowStyle = GetWindowLong(Hwnd(), GWL_STYLE);

		SetWindowLong(Hwnd(), GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(Hwnd(), &prevPlacement);
		SetWindowPos(Hwnd(), nullptr, NULL, NULL, NULL, NULL,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		isFullScreen = false;
	}

	SizeI AppWindow::GetMinimumSize() const noexcept
	{
		return minSize;
	}

	void AppWindow::SetMinimumSize(SizeI size) noexcept
	{
		minSize = size;
	}

	void AppWindow::SetTitle(std::wstring_view title) const noexcept
	{
		SendMessage(Hwnd(), WM_SETTEXT, NULL, std::bit_cast<LPARAM>(title.data()));
	}
	const std::wstring& AppWindow::GetTitle() const noexcept
	{
		return titleText;
	}

	Core::HandlerResult AppWindow::OnNCCreate(UINT, WPARAM, LPARAM lParam) noexcept
	{
		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		titleText = createStruct->lpszName;
		createStruct->dwExStyle |= WS_EX_NOREDIRECTIONBITMAP;

		return { 1, Core::HandlerResultFlag::PassToDefWindowProc };
	}

	Core::HandlerResult AppWindow::OnSetText(UINT, WPARAM, LPARAM lParam) noexcept
	{
		titleText = std::bit_cast<wchar_t*>(lParam);

		return { 1, Core::HandlerResultFlag::PassToDefWindowProc };
	}
	Core::HandlerResult AppWindow::OnGetText(UINT, WPARAM wParam, LPARAM lParam) const noexcept
	{
		auto size = std::min(titleText.size() + 1, wParam);
		wcsncpy_s(std::bit_cast<wchar_t*>(lParam), wParam, titleText.data(), size);
		return size;
	}
	Core::HandlerResult AppWindow::OnGetTextLength(UINT, WPARAM, LPARAM) const noexcept
	{
		return titleText.length();
	}
	Core::HandlerResult AppWindow::OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam) const noexcept
	{
		auto minMaxInfo = std::bit_cast<LPMINMAXINFO>(lParam);

		int frameX = GetSystemMetrics(SM_CXFRAME);
		int frameY = GetSystemMetrics(SM_CYFRAME);
		int padding = GetSystemMetrics(SM_CXPADDEDBORDER);
		minMaxInfo->ptMinTrackSize.y = minSize.cy + frameY + padding;

		minMaxInfo->ptMinTrackSize.x = minSize.cx + 2 * (frameX + padding);

		return 0;
	}
}
