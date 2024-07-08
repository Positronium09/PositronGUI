#pragma once

#include "Logger.hpp"
#include "Exceptions.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Size.hpp"
#include "WindowClass.hpp"
#include "helpers/HelperFunctions.hpp"
#include "helpers/EnumFlag.hpp"

#include <bit>
#include <set>
#include <ranges>
#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <Windows.h>


namespace PGUI::Core
{
	struct _handler_result_flag_values
	{
		enum EnumValues
		{
			Nothing = 0x00,
			NoFurtherHandling = 0x01,
			ForceThisResult = 0x02,
			ReturnPrevResult = 0x04,
			PassToDefWindowProc = 0x08
		};
	};
	using HandlerResultFlag = EnumFlag<_handler_result_flag_values>;

	struct HandlerResult
	{
		LRESULT result;
		HandlerResultFlag flags;

		HandlerResult(LRESULT _result, HandlerResultFlag _flags = HandlerResultFlag::Nothing) noexcept :
			result(_result), flags(_flags)
		{ }
	};

	struct WindowCreateParams
	{
		std::wstring windowName;
		PointI position;
		SizeI size;
		DWORD style;
		DWORD exStyle;

		WindowCreateParams(std::wstring_view _windowName,
			PointI _position, SizeI _size,
			DWORD _style, DWORD _exStyle=NULL) noexcept :
			windowName(_windowName),
			position(_position), size(_size),
			style(_style), exStyle(_exStyle)
		{ }
	};

	using Handler = std::function<HandlerResult(UINT, WPARAM, LPARAM)>;
	using HandlerMap = std::unordered_map<UINT, std::vector<Handler>>;

	class Window;

	template<typename T>
	concept IsWindowType = std::is_same_v<T, Window> || std::derived_from<T, Window>;

	template<IsWindowType T>
	using WindowPtr = std::shared_ptr<T>;

	using ChildWindowList = std::vector<WindowPtr<Window>>;

	using TimerId = UINT_PTR;
	using TimerCallback = std::function<void(TimerId)>;
	using TimerMap = std::unordered_map<TimerId, TimerCallback>;

	[[nodiscard]] Window* GetWindowFromHwnd(HWND hWnd) noexcept;

	class Window
	{
		friend LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		public:
		template <std::derived_from<Window> T, typename ...Args>
		[[nodiscard]] static WindowPtr<T> Create(const WindowCreateParams& createParams, Args... args)
		{
			auto window = std::make_shared<T>(args...);

			CreateWindowExW(createParams.exStyle, 
				window->windowClass->GetClassName().data(), createParams.windowName.data(),
				createParams.style,
				createParams.position.x, createParams.position.y, 
				createParams.size.cx, 
				createParams.size.cy,
				NULL, NULL, GetHInstance(),
				static_cast<LPVOID>(window.get()));

			if (window->hWnd == NULL)
			{
				ErrorHandling::Logger::Error(L"CreateWindow failed in Window::Create");
				auto errorCode = GetLastError();
				ErrorHandling::Logger::Error(std::format(L"Code: {}", errorCode));
				ErrorHandling::Logger::Error(GetWin32ErrorMessage(errorCode));
				throw ErrorHandling::Win32Exception{ };
			}
			
			const auto dpi = GetDpiForWindow(window->Hwnd());
			auto rect = window->GetWindowRect();

			rect.left = AdjustForDpi(rect.left, dpi);
			rect.top = AdjustForDpi(rect.top, dpi);
			rect.right = AdjustForDpi(rect.right, dpi);
			rect.bottom = AdjustForDpi(rect.bottom, dpi);

			window->MoveAndResize(rect);

			return window;
		}
		template <std::derived_from<Window> T, typename ...Args>
		WindowPtr<T> AddChildWindow(const WindowCreateParams& createParams, Args... args)
		{
			auto window = std::make_shared<T>(args...);

			CreateWindowExW(createParams.exStyle,
				window->windowClass->GetClassName().data(), createParams.windowName.data(),
				createParams.style | WS_CHILD,
				createParams.position.x, createParams.position.y,
				createParams.size.cx, createParams.size.cy,
				Hwnd(), NULL, GetHInstance(),
				static_cast<LPVOID>(window.get()));

			if (window->hWnd == NULL)
			{
				ErrorHandling::Logger::Error(L"CreateWindow failed in Window::AddChildWindow");
				auto errorCode = GetLastError();
				ErrorHandling::Logger::Error(std::format(L"Code: {}", errorCode));
				ErrorHandling::Logger::Error(GetWin32ErrorMessage(errorCode));
				throw ErrorHandling::Win32Exception{ };
			}
		
			childWindows.push_back(window);


			const auto dpi = GetDpiForWindow(window->Hwnd());

			auto rect = window->GetWindowRect();
			rect = PGUI::MapRect(nullptr, Hwnd(), rect);

			rect.left = AdjustForDpi(rect.left, dpi);
			rect.top = AdjustForDpi(rect.top, dpi);
			rect.right = AdjustForDpi(rect.right, dpi);
			rect.bottom = AdjustForDpi(rect.bottom, dpi);

			window->MoveAndResize(rect);

			return window;
		}
		template <std::derived_from<Window> T>
		WindowPtr<T> AddChildWindow(const WindowPtr<T> window)
		{
			auto style = GetWindowLongPtrW(window->Hwnd(), GWL_STYLE);
			SetWindowLongPtrW(window->Hwnd(), GWL_STYLE, static_cast<LONG_PTR>(style | WS_CHILD));

			SetParent(window->Hwnd(), Hwnd());

			childWindows.push_back(window);

			const auto dpi = GetDpiForWindow(window->Hwnd());

			auto rect = window->GetWindowRect();
			rect = PGUI::MapRect(nullptr, Hwnd(), rect);

			rect.left = AdjustForDpi(rect.left, dpi);
			rect.top = AdjustForDpi(rect.top, dpi);
			rect.right = AdjustForDpi(rect.right, dpi);
			rect.bottom = AdjustForDpi(rect.bottom, dpi);

			window->MoveAndResize(rect);

			return window;
		}

		void RemoveChildWindow(HWND childHwnd);

		explicit Window(const WindowClass::WindowClassPtr& wndClass) noexcept;
		virtual ~Window() noexcept;
		
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(Window&) noexcept = delete;
		
		explicit(false) operator HWND() const noexcept { return hWnd; }
		[[nodiscard]] HWND Hwnd() const noexcept { return hWnd; }
		[[nodiscard]] HWND ParentHwnd() const noexcept { return parenthWnd; }

		void Show(int show=SW_SHOW) const noexcept;

		void Move(PointL newPos) const noexcept;
		void Resize(SizeL newSize) const noexcept;
		void MoveAndResize(RectL newRect) const noexcept;
		void MoveAndResize(PointL newPos, SizeL newSize) const noexcept;

		[[nodiscard]] UINT GetDpi() const noexcept;

		[[nodiscard]] std::span<PointL> MapPoints(HWND hWndTo, std::span<PointL> points) const noexcept;
		[[nodiscard]] PointL MapPoint(HWND hWndTo, PointL point) const noexcept;

		[[nodiscard]] std::span<RectL> MapRects(HWND hWndTo, std::span<RectL> rects) const noexcept;
		[[nodiscard]] RectL MapRect(HWND hWndTo, RectL rect) const noexcept;

		TimerId AddTimer(TimerId id, std::chrono::milliseconds delay, 
			const TimerCallback& callback);
		void RemoveTimer(TimerId id);

		void Enable(bool enable) const noexcept;
		void AdjustForClientSize(SizeI size) const noexcept;
		void AdjustForRect(RectI rect) const noexcept;

		[[nodiscard]] const TimerMap& GetTimerMap() const noexcept;
		[[nodiscard]] TimerMap& GetTimerMap() noexcept;

		[[nodiscard]] PointL ScreenToClient(PointL point) const noexcept;
		[[nodiscard]] RectL ScreenToClient(RectL rect) const noexcept;

		[[nodiscard]] PointL ClientToScreen(PointL point) const noexcept;
		[[nodiscard]] RectL ClientToScreen(RectL rect) const noexcept;

		[[nodiscard]] WindowClass::WindowClassPtr GetWindowClass() const noexcept;
		[[nodiscard]] const ChildWindowList& GetChildWindowList() const noexcept;

		[[nodiscard]] RectL GetWindowRect() const noexcept;
		[[nodiscard]] RectL GetClientRect() const noexcept;
		[[nodiscard]] SizeL GetWindowSize() const noexcept;
		[[nodiscard]] SizeL GetClientSize() const noexcept;

		void Invalidate() const noexcept;

		[[nodiscard]] bool operator==(const Window& other) const noexcept;

		protected:
		virtual void RegisterMessageHandler(UINT msg, const Handler& handler) noexcept final;
		template <typename T>
		void RegisterMessageHandler(UINT msg, HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM)) noexcept
		{
			RegisterMessageHandler(msg, std::bind_front(memberFunction, std::bit_cast<T*>(this)));
		}
		template <typename T>
		void RegisterMessageHandler(UINT msg, HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM) const) noexcept
		{
			RegisterMessageHandler(msg, std::bind_front(memberFunction, std::bit_cast<const T*>(this)));
		}

		virtual HandlerResult OnDPIChange(float dpiScale, RectI suggestedRect);
		virtual void AdjustChildWindowsForDPI(float dpiScale);

		private:
		HWND hWnd = nullptr;
		HWND parenthWnd = nullptr;
		ChildWindowList childWindows;

		UINT prevDpi = USER_DEFAULT_SCREEN_DPI;

		HandlerMap handlerMap;
		TimerMap timerMap;
		WindowClass::WindowClassPtr windowClass;
		
		HandlerResult OnDPIChanged(UINT msg, WPARAM wParam, LPARAM lParam);
	};

	LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
