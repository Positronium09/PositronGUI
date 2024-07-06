#include "core/Window.hpp"

#include <bit>


namespace PGUI::Core
{
	Window* GetWindowFromHwnd(HWND hWnd) noexcept
	{
		return std::bit_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	void Window::Invalidate() const noexcept
	{
		InvalidateRect(hWnd, nullptr, false);
	}

	void Window::RemoveChildWindow(HWND childHwnd)
	{
		for (const auto& [index, child] : std::views::enumerate(childWindows))
		{
			if (child->Hwnd() == childHwnd)
			{
				auto iter = childWindows.begin();
				std::advance(iter, index);
				childWindows.erase(iter);

				SetParent(childHwnd, NULL);

				LONG_PTR style = GetWindowLongPtrW(childHwnd, GWL_STYLE);
				style &= ~(WS_CHILD);
				style |= WS_POPUP;
				SetWindowLongPtrW(childHwnd, GWL_STYLE, style);

				break;
			}
		}
	}

	Window::Window(const WindowClass::WindowClassPtr& wndClass) noexcept :
		windowClass{ wndClass }
	{
		RegisterMessageHandler(WM_DPICHANGED, &Window::OnDPIChanged);
	}

	Window::~Window() noexcept
	{
		for (const auto& [id, _] : timerMap)
		{
			KillTimer(Hwnd(), id);
		}
		DestroyWindow(hWnd);
	}

	void Window::Show(int show) const noexcept
	{
		ShowWindow(hWnd, show);
	}

	WindowClass::WindowClassPtr Window::GetWindowClass() const noexcept
	{
		return windowClass;
	}

	const ChildWindowList& Window::GetChildWindowList() const noexcept
	{
		return childWindows;
	}

	RectL Window::GetWindowRect() const noexcept
	{
		RECT windowRect{ };
		::GetWindowRect(hWnd, &windowRect);
		return windowRect;
	}

	RectL Window::GetClientRect() const noexcept
	{
		RECT clientRect{ };
		::GetClientRect(hWnd, &clientRect);
		return clientRect;
	}

	void Window::Move(PointL newPos) const noexcept
	{
		SetWindowPos(Hwnd(), nullptr, newPos.x, newPos.y, NULL, NULL, 
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Window::Resize(SizeL newSize) const noexcept
	{
		SetWindowPos(Hwnd(), nullptr, NULL, NULL, newSize.cx, newSize.cy, 
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Window::MoveAndResize(RectL newRect) const noexcept
	{
		const auto size = newRect.Size();

		SetWindowPos(Hwnd(),
			nullptr,
			newRect.left,
			newRect.top,
			size.cx,
			size.cy,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Window::MoveAndResize(PointL newPos, SizeL newSize) const noexcept
	{
		SetWindowPos(Hwnd(),
			nullptr,
			newPos.x,
			newPos.y,
			newSize.cx,
			newSize.cy,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	UINT Window::GetDpi() const noexcept
	{
		return GetDpiForWindow(Hwnd());
	}

	std::span<PointL> Window::MapPoints(HWND hWndTo, std::span<PointL> points) const noexcept
	{
		return PGUI::MapPoints(Hwnd(), hWndTo, points);
	}

	PointL Window::MapPoint(HWND hWndTo, PointL point) const noexcept
	{
		return PGUI::MapPoint(Hwnd(), hWndTo, point);
	}

	std::span<RectL> Window::MapRects(HWND hWndTo, std::span<RectL> rects) const noexcept
	{
		return PGUI::MapRects(Hwnd(), hWndTo, rects);
	}

	RectL Window::MapRect(HWND hWndTo, RectL rect) const noexcept
	{
		return PGUI::MapRect(Hwnd(), hWndTo, rect);
	}

	TimerId Window::AddTimer(TimerId id, std::chrono::milliseconds delay, 
		const TimerCallback& callback)
	{
		 if (TimerId setTimerId = 
			 SetTimer(Hwnd(), id, static_cast<UINT>(delay.count()), nullptr); 
			 setTimerId == 0)
		 {
			 Core::ErrorHandling::Logger::Error(GetWin32ErrorMessage());
			 return setTimerId;
		 }

		 timerMap[id] = callback;

		 return id;
	}

	void Window::RemoveTimer(TimerId id)
	{
		if (auto succeeded = KillTimer(Hwnd(), id);
			succeeded == 0)
		{
			if (DWORD error = GetLastError();
				error != ERROR_SUCCESS)
			{
				Core::ErrorHandling::Logger::Error(GetWin32ErrorMessage(error));
				return;
			}
		}

		timerMap.erase(id);
	}

	void Window::Enable(bool enable) const noexcept
	{
		EnableWindow(Hwnd(), static_cast<BOOL>(enable));
	}

	void Window::AdjustForClientSize(SizeI size) const noexcept
	{
		RECT rc;
		SetRect(&rc, 0, 0, size.cx, size.cy);
		
		AdjustWindowRectExForDpi(&rc,
			static_cast<DWORD>(GetWindowLongPtrW(hWnd, GWL_STYLE)), 
			FALSE,
			static_cast<DWORD>(GetWindowLongPtrW(hWnd, GWL_EXSTYLE)), 
			GetDpi());

		RectL r = rc;
		Resize(r.Size());
	}

	void Window::AdjustForRect(RectI rect) const noexcept
	{
		RECT rc = rect;
		auto dpi = GetDpi();

		AdjustWindowRectExForDpi(&rc,
			static_cast<DWORD>(GetWindowLongPtrW(hWnd, GWL_STYLE)),
			FALSE,
			static_cast<DWORD>(GetWindowLongPtrW(hWnd, GWL_EXSTYLE)),
			dpi);

		MoveAndResize(rc);
	}

	const TimerMap& Window::GetTimerMap() const noexcept
	{
		return timerMap;
	}

	TimerMap& Window::GetTimerMap() noexcept
	{
		return timerMap;
	}

	PointL Window::ScreenToClient(PointL point) const noexcept
	{
		POINT p = point;
		::ScreenToClient(hWnd, &p);

		return p;
	}

	RectL Window::ScreenToClient(RectL rect) const noexcept
	{
		RECT rc = rect;
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	PointL Window::ClientToScreen(PointL point) const noexcept
	{
		POINT p = point;
		::ClientToScreen(hWnd, &p);

		return p;
	}

	RectL Window::ClientToScreen(RectL rect) const noexcept
	{
		RECT rc = rect;
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	SizeL Window::GetWindowSize() const noexcept
	{
		return GetWindowRect().Size();
	}

	SizeL Window::GetClientSize() const noexcept
	{
		return GetClientRect().Size();
	}

	void Window::RegisterMessageHandler(UINT msg, const Handler& handler) noexcept
	{
		handlerMap[msg].push_back(handler);
	}
	
	HandlerResult Window::OnDPIChange(float dpiScale, RectI suggestedRect)
	{
		MoveAndResize(suggestedRect);

		ErrorHandling::Logger::Log(std::format(L"{}", dpiScale));
		AdjustChildWindowsForDPI(dpiScale);

		return 0;
	}

	void Window::AdjustChildWindowsForDPI(float dpiScale)
	{
		for (const auto& child : childWindows)
		{
			auto rc = child->GetWindowRect();

			rc = PGUI::MapRect(nullptr, Hwnd(), rc);

			#pragma warning (push)
			#pragma warning (disable : 4244)

			rc.left *= dpiScale;
			rc.top *= dpiScale;
			rc.right *= dpiScale;
			rc.bottom *= dpiScale;

			#pragma warning (pop)

			child->OnDPIChange(dpiScale, rc);
		}
	}

	Core::HandlerResult Window::OnDPIChanged(UINT, WPARAM wParam, LPARAM lParam)
	{
		const auto result = OnDPIChange(
			static_cast<float>(LOWORD(wParam)) /
			static_cast<float>(prevDpi),
			*std::bit_cast<LPRECT>(lParam));

		prevDpi = LOWORD(wParam);

		return result;
	}

	bool Window::operator==(const Window& other) const noexcept
	{
		return hWnd == other.hWnd;
	}

	LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

			SetWindowLongPtrW(hWnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(createStruct->lpCreateParams));
			auto window = GetWindowFromHwnd(hWnd);

			window->hWnd = hWnd;
			window->parenthWnd = createStruct->hwndParent;
			window->prevDpi = window->GetDpi();
		}

		auto window = GetWindowFromHwnd(hWnd);

		LRESULT result = 0;
		if (!window)
		{
			return result;
		}

		if (msg == WM_TIMER)
		{
			if (auto id = wParam;
				window->timerMap.contains(id))
			{
				window->timerMap.at(id)(id);
				return 0;
			}
		}

		else if (!window->handlerMap.contains(msg))
		{
			result = DefWindowProcW(hWnd, msg, wParam, lParam);
			return result;
		}

		bool passToDefWindowProc = false;
		bool forceCurrentResult = false;
		for (auto const& handler : window->handlerMap[msg])
		{
			auto [lResult, flags] = handler(msg, wParam, lParam);
			using enum HandlerResultFlag::EnumValues;

			if (!(flags & ReturnPrevResult))
			{
				result = lResult;
			}
			if (flags & NoFurtherHandling)
			{
				break;
			}
			if (forceCurrentResult)
			{
				continue;
			}
			if (flags & ForceThisResult)
			{
				forceCurrentResult = true;
			}
			if (flags & PassToDefWindowProc)
			{
				passToDefWindowProc = true;
			}
		}
		if (passToDefWindowProc)
		{
			DefWindowProcW(hWnd, msg, wParam, lParam);
		}
		

		if (msg == WM_NCCREATE || msg == WM_PAINT)
		{
			DefWindowProcW(hWnd, msg, wParam, lParam);
		}

		return result;
	}
}
