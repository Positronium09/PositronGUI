#include "core/Window.hpp"

#include <bit>


namespace PGUI::Core
{
	Window* GetWindowFromHwnd(HWND hWnd)
	{
		return std::bit_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	void Window::Invalidate() const
	{
		InvalidateRect(hWnd, nullptr, false);
	}

	Window::Window(const WindowClass::WindowClassPtr& wndClass) :
		windowClass{ wndClass }
	{
	}

	Window::~Window()
	{
		DestroyWindow(hWnd);
	}

	void Window::Show(int show)
	{
		ShowWindow(hWnd, show);
	}

	const ChildWindowList& Window::GetChildWindowList() const
	{
		return childWindows;
	}

	RectL Window::GetWindowRect() const
	{
		RECT windowRect{ };
		::GetWindowRect(hWnd, &windowRect);
		return windowRect;
	}

	RectL Window::GetClientRect() const
	{
		RECT clientRect{ };
		::GetClientRect(hWnd, &clientRect);
		return clientRect;
	}

	void Window::Move(PointL newPos) const
	{
		SetWindowPos(Hwnd(), nullptr, newPos.x, newPos.y, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::Resize(SizeL newSize) const
	{
		SetWindowPos(Hwnd(), nullptr, NULL, NULL, newSize.cx, newSize.cy, SWP_NOMOVE | SWP_NOZORDER);
	}

	PointL Window::ScreenToClient(PointL point) const
	{
		POINT p = point;
		::ScreenToClient(hWnd, &p);

		return p;
	}

	RectL Window::ScreenToClient(RectL rect) const
	{
		RECT rc = rect;
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ScreenToClient(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	PointL Window::ClientToScreen(PointL point) const
	{
		POINT p = point;
		::ClientToScreen(hWnd, &p);

		return p;
	}

	RectL Window::ClientToScreen(RectL rect) const
	{
		RECT rc = rect;
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc));
		::ClientToScreen(hWnd, std::bit_cast<LPPOINT>(&rc.right));

		return rc;
	}

	SizeL Window::GetWindowSize() const
	{
		return GetWindowRect().Size();
	}

	SizeL Window::GetClientSize() const
	{
		return GetClientRect().Size();
	}

	void Window::RegisterMessageHandler(UINT msg, const Handler& handler)
	{
		handlerMap[msg].push_back(handler);
	}

	bool Window::operator==(const Window& other) const
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
