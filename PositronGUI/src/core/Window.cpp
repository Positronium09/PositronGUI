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

	Window::Window(const WindowClass::WindowClassPtr& wndClass) noexcept :
		windowClass{ wndClass }
	{
	}

	Window::~Window() noexcept
	{
		DestroyWindow(hWnd);
	}

	void Window::Show(int show) noexcept
	{
		ShowWindow(hWnd, show);
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
		SetWindowPos(Hwnd(), nullptr, newPos.x, newPos.y, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::Resize(SizeL newSize) const noexcept
	{
		SetWindowPos(Hwnd(), nullptr, NULL, NULL, newSize.cx, newSize.cy, SWP_NOMOVE | SWP_NOZORDER);
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
		}

		auto window = GetWindowFromHwnd(hWnd);

		LRESULT result = 0;
		if (!window)
		{
			return result;
		}

		if (!window->handlerMap.contains(msg))
		{
			result = DefWindowProcW(hWnd, msg, wParam, lParam);
			return result;
		}

		bool passToDefWindowProc = false;
		bool forceCurrentResult = false;
		for (auto const& handler : window->handlerMap[msg])
		{
			using enum PGUI::Core::HandlerResultFlags;
			auto [lResult, flags] = handler(msg, wParam, lParam);

			if (!forceCurrentResult)
			{
				if (!(flags & ReturnPrevResult))
				{
					result = lResult;
				}
				if (flags & ForceThisResult)
				{
					forceCurrentResult = true;
				}
			}
			if (flags & NoFurtherHandling)
			{
				break;
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
