#include "core/Window.hpp"

#include <bit>
#include <algorithm>
#include <ranges>


namespace PGUI::Core
{
	auto GetWindowFromHwnd(HWND hWnd) noexcept -> WindowPtr<Window>
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

				SetParent(childHwnd, nullptr);

				LONG_PTR style = GetWindowLongPtrW(childHwnd, GWL_STYLE);
				style &= ~(WS_CHILD);
				style |= WS_POPUP;
				SetWindowLongPtrW(childHwnd, GWL_STYLE, style);

				OnChildRemoved();

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

	auto Window::IsVisible() const noexcept -> bool
	{
		return IsWindowVisible(Hwnd());
	}

	auto Window::GetWindowRect() const noexcept -> RectL
	{
		RECT windowRect{ };
		::GetWindowRect(hWnd, &windowRect);
		return windowRect;
	}

	auto Window::GetClientRect() const noexcept -> RectL
	{
		RECT clientRect{ };
		::GetClientRect(hWnd, &clientRect);
		return clientRect;
	}

	auto Window::GetClientRectWithoutDPI() const noexcept -> RectL
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

	auto Window::GetDPI() const noexcept -> UINT32
	{
		return GetDpiForWindow(Hwnd());
	}

	auto Window::GetDpiScaleTransform(std::optional<PointF> center) const noexcept -> D2D1_MATRIX_3X2_F
	{
		auto dpi = static_cast<float>(GetDPI());
		return D2D1::Matrix3x2F::Scale(dpi / DEFAULT_SCREEN_DPI, dpi / DEFAULT_SCREEN_DPI,
				center.value_or(GetClientRect().Center()));
	}

	auto Window::MapPoints(HWND hWndTo, std::span<PointL> points) const noexcept -> std::span<PointL>
	{
		return PGUI::MapPoints(Hwnd(), hWndTo, points);
	}

	auto Window::MapPoint(HWND hWndTo, PointL point) const noexcept -> PointL
	{
		return PGUI::MapPoint(Hwnd(), hWndTo, point);
	}

	auto Window::MapRects(HWND hWndTo, std::span<RectL> rects) const noexcept -> std::span<RectL>
	{
		return PGUI::MapRects(Hwnd(), hWndTo, rects);
	}

	auto Window::MapRect(HWND hWndTo, RectL rect) const noexcept -> RectL
	{
		return PGUI::MapRect(Hwnd(), hWndTo, rect);
	}

	auto Window::ChildWindowFromPoint(PointL point, UINT flags) const noexcept -> WindowPtr<Window>
	{
		WindowPtr<Window> wnd = nullptr;
		if (HWND hwnd = ChildWindowFromPointEx(Hwnd(), point, flags);
			hwnd != nullptr)
		{
			return GetWindowFromHwnd(hwnd);
		}
		return wnd;
	}

	auto Window::AddTimer(TimerId id, std::chrono::milliseconds delay, 
		std::optional<TimerCallback> callback) noexcept -> TimerId
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

	auto Window::ScreenToClient(PointL point) const noexcept -> PointL
	{
		POINT p = point;
		::ScreenToClient(hWnd, &p);

		return p;
	}

	auto Window::ScreenToClient(RectL rect) const noexcept -> RectL
	{
		RECT rc = rect;
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	auto Window::ClientToScreen(PointL point) const noexcept -> PointL
	{
		POINT p = point;
		::ClientToScreen(hWnd, &p);

		return p;
	}

	auto Window::ClientToScreen(RectL rect) const noexcept -> RectL
	{
		RECT rc = rect;
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	auto Window::GetWindowSize() const noexcept -> SizeL
	{
		return GetWindowRect().Size();
	}

	auto Window::GetClientSize() const noexcept -> SizeL
	{
		return GetClientRect().Size();
	}

	auto Window::GetClientSizeWithoutDPI() const noexcept -> SizeL
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

	auto Window::OnDPIChange(float dpiScale, RectI suggestedRect) -> HandlerResult
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

	void Window::OnChildAdded(WindowPtr<Window> /* unused */)
	{
		/* not implemented */
	}

	void Window::OnChildRemoved()
	{
		/* not implemented */
	}

	auto Window::OnDPIChanged(UINT, WPARAM wParam, LPARAM lParam) -> HandlerResult
	{
		const auto result = OnDPIChange(
			static_cast<float>(LOWORD(wParam)) /
			static_cast<float>(prevDpi),
			*std::bit_cast<LPRECT>(lParam));

		prevDpi = LOWORD(wParam);

		return result;
	}

	auto Window::operator==(const Window& other) const noexcept -> bool
	{
		return hWnd == other.hWnd;
	}

	auto _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		if (msg == WM_NCCREATE)
		{
			auto* createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

			SetWindowLongPtrW(hWnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(createStruct->lpCreateParams));
			auto* window = GetWindowFromHwnd(hWnd);

			window->hWnd = hWnd;
			window->parenthWnd = createStruct->hwndParent;
			window->prevDpi = window->GetDPI();
		}

		auto* window = GetWindowFromHwnd(hWnd);

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
