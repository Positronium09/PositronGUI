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
		RegisterMessageHandler(WM_LBUTTONDOWN, &AppWindow::OnLButtonDown);
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

	bool AppWindow::IsResizable() const noexcept
	{
		return GetWindowLongPtrW(Hwnd(), GWL_STYLE) & WS_SIZEBOX;
	}
	void AppWindow::SetResizeable(bool isResizable) const noexcept
	{
		auto style = GetWindowLongPtrW(Hwnd(), GWL_STYLE);
		if (isResizable)
		{
			style |= WS_SIZEBOX;
		}
		else
		{
			style &= ~WS_SIZEBOX;
		}
		SetWindowLongPtrW(Hwnd(), GWL_STYLE, style);
	}

	bool AppWindow::IsMaximizable() const noexcept
	{
		return GetWindowLongPtrW(Hwnd(), GWL_STYLE) & WS_MAXIMIZEBOX;
	}
	void AppWindow::SetMaximizable(bool isMaximizable) const noexcept
	{
		auto style = GetWindowLongPtrW(Hwnd(), GWL_STYLE);
		if (isMaximizable)
		{
			style |= WS_MAXIMIZEBOX;
		}
		else
		{
			style &= ~WS_MAXIMIZEBOX;
		}
		SetWindowLongPtrW(Hwnd(), GWL_STYLE, style);
	}

	bool AppWindow::IsMinimizable() const noexcept
	{
		return GetWindowLongPtrW(Hwnd(), GWL_STYLE) & WS_MINIMIZEBOX;
	}
	void AppWindow::SetMinimizable(bool isMinimizable) const noexcept
	{
		auto style = GetWindowLongPtrW(Hwnd(), GWL_STYLE);
		if (isMinimizable)
		{
			style |= WS_MINIMIZEBOX;
		}
		else
		{
			style &= ~WS_MINIMIZEBOX;
		}
		SetWindowLongPtrW(Hwnd(), GWL_STYLE, style);
	}

	bool AppWindow::IsAlwaysOnTop() const noexcept
	{
		return GetWindowLongPtrW(Hwnd(), GWL_EXSTYLE) & WS_EX_TOPMOST;
	}
	void AppWindow::SetAlwaysOnTop(bool isAlwaysOnTop) const noexcept
	{
		auto style = GetWindowLongPtrW(Hwnd(), GWL_EXSTYLE);
		if (isAlwaysOnTop)
		{
			style |= WS_EX_TOPMOST;
		}
		else
		{
			style &= ~WS_EX_TOPMOST;
		}
		SetWindowLongPtrW(Hwnd(), GWL_EXSTYLE, style);
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
	Core::HandlerResult AppWindow::OnLButtonDown(UINT, WPARAM, LPARAM) const noexcept
	{
		SetFocus(Hwnd());

		return 0;
	}
}
