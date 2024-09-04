#include "core/Window.hpp"

#include <bit>
#include <algorithm>
#include <ranges>


namespace PGUI::Core
{
	WindowPtr<Window> GetWindowFromHwnd(HWND hWnd) noexcept
	{
		return std::bit_cast<WindowPtr<Window>>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
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
		std::ranges::for_each(timerMap | std::views::keys,
			[this](const auto& id)
		{
			KillTimer(Hwnd(), id);
		});
		DestroyWindow(hWnd);
	}

	void Window::Show(int show) const noexcept
	{
		ShowWindow(hWnd, show);
	}

	bool Window::IsVisible() const noexcept
	{
		return IsWindowVisible(Hwnd());
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

	RectL Window::GetClientRectWithoutDPI() const noexcept
	{
		RectF rect = GetClientRect();
		auto topLeft = rect.TopLeft();
		SizeL size = UnScaleByDPI<SizeF>(GetClientSize());
		return RectL{ topLeft, size };
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

	UINT Window::GetDPI() const noexcept
	{
		return GetDpiForWindow(Hwnd());
	}

	D2D1_MATRIX_3X2_F Window::GetDpiScaleTransform(std::optional<PointF> center) const noexcept
	{
		auto dpi = static_cast<float>(GetDPI());
		return D2D1::Matrix3x2F::Scale(dpi / DEFAULT_SCREEN_DPI, dpi / DEFAULT_SCREEN_DPI,
				center.value_or(GetClientRect().Center()));
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

	WindowPtr<Window> Window::ChildWindowFromPoint(PointL point, UINT flags) const noexcept
	{
		WindowPtr<Window> wnd = nullptr;
		if (HWND hwnd = ChildWindowFromPointEx(Hwnd(), point, flags);
			hwnd != nullptr)
		{
			return GetWindowFromHwnd(hwnd);
		}
		return wnd;
	}

	TimerId Window::AddTimer(TimerId id, std::chrono::milliseconds delay, 
		std::optional<TimerCallback> callback) noexcept
	{
		 if (TimerId setTimerId = 
			 SetTimer(Hwnd(), id, static_cast<UINT>(delay.count()), nullptr); 
			 setTimerId == 0)
		 {
			 auto errCode = GetLastError();
			 HR_L(HresultFromWin32(errCode));
			 return setTimerId;
		 }

		 if (callback.has_value())
		 {
			 timerMap[id] = *callback;
		 }

		 return id;
	}

	void Window::RemoveTimer(TimerId id) noexcept
	{
		if (!HasTimer(id))
		{
			return;
		}

		if (auto succeeded = KillTimer(Hwnd(), id);
			succeeded == 0)
		{
			if (DWORD error = GetLastError();
				error != ERROR_SUCCESS)
			{
				HR_L(HresultFromWin32(error));
				return;
			}
		}

		if (timerMap.contains(id))
		{
			timerMap.erase(id);
		}
	}

	bool Window::HasTimer(TimerId id) const noexcept
	{
		return timerMap.contains(id);
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
			GetDPI());

		RectL r = rc;
		Resize(r.Size());
	}

	void Window::AdjustForRect(RectI rect) const noexcept
	{
		RECT rc = rect;
		auto dpi = GetDPI();

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

	SizeL Window::GetClientSizeWithoutDPI() const noexcept
	{
		return GetClientRectWithoutDPI().Size();
	}

	void Window::RegisterMessageHandler(UINT msg, const Handler& handler) noexcept
	{
		handlerMap[msg].push_back(handler);
	}
	
	void Window::RemoveGeneralHandler()
	{
		generalHandler.reset();
	}

	HandlerResult Window::OnDPIChange(float dpiScale, RectI suggestedRect)
	{
		MoveAndResize(suggestedRect);

		Logger::Log(std::format(L"{}", dpiScale));
		AdjustChildWindowsForDPI(dpiScale);

		return 0;
	}

	void Window::AdjustChildWindowsForDPI(float dpiScale)
	{
		for (const auto& child : childWindows)
		{
			auto rc = child->GetWindowRect();

			rc = PGUI::MapRect(nullptr, Hwnd(), rc);

			rc.left = static_cast<long>(
				static_cast<float>(rc.left) * dpiScale);

			rc.top = static_cast<long>(
				static_cast<float>(rc.top) * dpiScale);
			
			rc.right = static_cast<long>(
				static_cast<float>(rc.right) * dpiScale);
			
			rc.bottom = static_cast<long>(
				static_cast<float>(rc.bottom) * dpiScale);

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
			window->prevDpi = window->GetDPI();
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
				std::invoke(window->timerMap.at(id), id);
				return 0;
			}
		}

		else if (!window->handlerMap.contains(msg))
		{
			if (window->generalHandler.has_value())
			{
				result = window->generalHandler.value()(msg, wParam, lParam).result;
				return result;
			}
			result = DefWindowProcW(hWnd, msg, wParam, lParam);
			return result;
		}

		bool passToDefWindowProc = false;
		bool forceCurrentResult = false;
		for (auto const& handler : window->handlerMap[msg])
		{
			auto [lResult, flags] = std::invoke(handler, msg, wParam, lParam);
			using enum HandlerResultFlag;

			if (!(flags & ReturnPrevResult))
			{
				result = lResult;
			}
			if ((flags & NoFurtherHandling) != Nothing)
			{
				break;
			}
			if (forceCurrentResult)
			{
				continue;
			}
			if ((flags & ForceThisResult) != Nothing)
			{
				forceCurrentResult = true;
			}
			if ((flags & PassToDefWindowProc) != Nothing)
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
