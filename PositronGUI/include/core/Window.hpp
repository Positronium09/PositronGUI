#pragma once

#include "Logger.hpp"
#include "Exceptions.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Size.hpp"
#include "WindowClass.hpp"
#include "helpers/HelperFunctions.hpp"
#include "helpers/ScopedTimer.hpp"
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
	constexpr UINT DEFAULT_SCREEN_DPI = USER_DEFAULT_SCREEN_DPI;

	enum class HandlerResultFlag
	{
		Nothing = 0x00,
		NoFurtherHandling = 0x01,
		ForceThisResult = 0x02,
		ReturnPrevResult = 0x04,
		PassToDefWindowProc = 0x08
	};

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
			DWORD _style, DWORD _exStyle=NULL) :
			windowName(_windowName),
			position(_position), size(_size),
			style(_style), exStyle(_exStyle)
		{ }
	};

	using Handler = std::function<HandlerResult(UINT, WPARAM, LPARAM)>;
	using HandlerMap = std::unordered_map<UINT, std::vector<Handler>>;

	class Window;

	template<typename T>
	concept WindowType = std::is_base_of_v<Window, T>;

	template<WindowType T>
	using WindowOwnPtr = std::unique_ptr<T>;
	template<WindowType T>
	using WindowPtr = T*;
	template<WindowType T>
	using CWindowPtr = const T*;

	using ChildWindowList = std::vector<WindowOwnPtr<Window>>;

	using TimerId = UINT_PTR;
	using TimerCallback = std::function<void(TimerId)>;
	using TimerMap = std::unordered_map<TimerId, TimerCallback>;

	[[nodiscard]] auto GetWindowFromHwnd(HWND hWnd) noexcept -> WindowPtr<Window>;

	class Window
	{
		friend auto _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT;

		public:
		template <WindowType T, typename ...Args>
		[[nodiscard]] static auto Create(const WindowCreateParams& createParams, Args&&... args) -> WindowOwnPtr<T>
		{
			auto window = std::make_unique<T>(std::forward<Args>(args)...);
			auto wnd = window.get();

			auto size = AdjustForDPI(SizeF{ createParams.size }, static_cast<float>(GetDpiForSystem()));
			RECT rc = RectI{ PointI{ }, size };
			AdjustWindowRectExForDpi(&rc, createParams.style, FALSE, createParams.exStyle, GetDpiForSystem());
			RectI rect = rc;
			auto scaledSize = rect.Size();

			CreateWindowExW(createParams.exStyle, 
				window->windowClass->ClassName().data(), createParams.windowName.data(),
				createParams.style,
				createParams.position.x,
				createParams.position.y,
				scaledSize.cx, scaledSize.cy,
				NULL, NULL, GetHInstance(),
				static_cast<LPVOID>(wnd));

			if (window->hWnd == NULL)
			{
				auto errCode = GetLastError();
				HR_L(HresultFromWin32(errCode));
				throw Win32Exception{ errCode };
			}

			return window;
		}
		template <WindowType T, typename ...Args>
		auto AddChildWindow(const WindowCreateParams& createParams, Args&&... args) -> WindowPtr<T>
		{
			auto window = std::make_unique<T>(std::forward<Args>(args)...);
			auto wnd = window.get();

			auto size = AdjustForDPI(SizeF{ createParams.size }, static_cast<float>(GetDpiForSystem()));
			RECT rc = RectI{ createParams.position, size };
			AdjustWindowRectExForDpi(&rc, createParams.style, FALSE, createParams.exStyle, GetDpiForSystem());
			RectI rect = rc;
			PointI pos = AdjustForDPI(PointF{ rect.TopLeft() }, static_cast<float>(GetDpiForSystem()));
			auto scaledSize = rect.Size();

			CreateWindowExW(createParams.exStyle,
				window->windowClass->ClassName().data(), createParams.windowName.data(),
				createParams.style | WS_CHILD,
				pos.x, pos.y,
				scaledSize.cx, scaledSize.cy,
				Hwnd(), NULL, GetHInstance(),
				static_cast<LPVOID>(wnd));

			if (window->hWnd == NULL)
			{
				auto errCode = GetLastError();
				HR_L(HresultFromWin32(errCode));
				throw Win32Exception{ errCode };
			}
		
			childWindows.push_back(std::move(window));

			OnChildAdded(wnd);

			return wnd;
		}
		template <WindowType T>
		auto AddChildWindow(const WindowOwnPtr<T> window) -> WindowPtr<T>
		{
			auto wnd = window.get();

			auto style = GetWindowLongPtrW(wnd->Hwnd(), GWL_STYLE);
			SetWindowLongPtrW(wnd->Hwnd(), GWL_STYLE, static_cast<LONG_PTR>(style | WS_CHILD));

			SetParent(wnd->Hwnd(), Hwnd());

			childWindows.push_back(std::move(window));

			OnChildAdded(wnd);

			return wnd;
		}

		void RemoveChildWindow(HWND childHwnd);

		explicit Window(const WindowClass::WindowClassPtr& wndClass) noexcept;
		virtual ~Window() noexcept;
		
		Window(const Window&) = delete;
		auto operator=(const Window&) -> Window& = delete;
		Window(Window&&) noexcept = delete;
		auto operator=(Window&&) noexcept -> Window& = delete;
		
		explicit(false) operator HWND() const noexcept { return hWnd; }
		[[nodiscard]] auto Hwnd() const noexcept { return hWnd; }
		[[nodiscard]] auto ParentHwnd() const noexcept { return parenthWnd; }

		void Show(int show=SW_SHOW) const noexcept;
		[[nodiscard]] auto IsVisible() const noexcept -> bool;

		void Move(PointL newPos) const noexcept;
		void Resize(SizeL newSize) const noexcept;
		void MoveAndResize(RectL newRect) const noexcept;
		void MoveAndResize(PointL newPos, SizeL newSize) const noexcept;

		[[nodiscard]] auto GetDPI() const noexcept -> UINT;
		[[nodiscard]] auto GetDpiScaleTransform(std::optional<PointF> center = std::nullopt) const noexcept -> D2D1_MATRIX_3X2_F;

		[[nodiscard]] auto MapPoints(HWND hWndTo, std::span<PointL> points) const noexcept -> std::span<PointL>;
		[[nodiscard]] auto MapPoint(HWND hWndTo, PointL point) const noexcept -> PointL;

		[[nodiscard]] auto MapRects(HWND hWndTo, std::span<RectL> rects) const noexcept -> std::span<RectL>;
		[[nodiscard]] auto MapRect(HWND hWndTo, RectL rect) const noexcept -> RectL;

		[[nodiscard]] auto ChildWindowFromPoint(PointL point, UINT flags) const noexcept -> WindowPtr<Window>;

		template <typename T>
		[[nodiscard]] auto ScaleByDPI(T val) const noexcept
		{
			return static_cast<T>(val * 
				static_cast<float>(GetDPI()) / Core::DEFAULT_SCREEN_DPI);
		}
		template <typename T>
		[[nodiscard]] auto UnScaleByDPI(T val) const noexcept
		{
			return static_cast<T>(val *
				Core::DEFAULT_SCREEN_DPI / static_cast<float>(GetDPI()));
		}

		auto AddTimer(TimerId id, std::chrono::milliseconds delay, 
			std::optional<TimerCallback> callback = std::nullopt) noexcept -> TimerId;
		void RemoveTimer(TimerId id) noexcept;
		[[nodiscard]] auto HasTimer(TimerId id) const noexcept { return timerMap.contains(id); }

		void Enable(bool enable) const noexcept;
		void AdjustForClientSize(SizeI size) const noexcept;
		void AdjustForRect(RectI rect) const noexcept;

		[[nodiscard]] auto& GetTimerMap() const noexcept { return timerMap; }
		[[nodiscard]] auto& GetTimerMap() noexcept { return timerMap; }

		[[nodiscard]] auto ScreenToClient(PointL point) const noexcept -> PointL;
		[[nodiscard]] auto ScreenToClient(RectL rect) const noexcept -> RectL;

		[[nodiscard]] auto ClientToScreen(PointL point) const noexcept -> PointL;
		[[nodiscard]] auto ClientToScreen(RectL rect) const noexcept -> RectL;

		[[nodiscard]] auto GetWindowClass() const noexcept { return windowClass; }
		[[nodiscard]] auto& GetChildWindowList() const noexcept { return childWindows; }

		[[nodiscard]] auto GetWindowRect() const noexcept -> RectL;
		[[nodiscard]] auto GetClientRect() const noexcept -> RectL;
		[[nodiscard]] auto GetClientRectWithoutDPI() const noexcept -> RectL;
		[[nodiscard]] auto GetWindowSize() const noexcept -> SizeL;
		[[nodiscard]] auto GetClientSize() const noexcept -> SizeL;
		[[nodiscard]] auto GetClientSizeWithoutDPI() const noexcept -> SizeL;

		void Invalidate() const noexcept;

		[[nodiscard]] auto operator==(const Window& other) const noexcept -> bool;

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

		template <typename T>
		void RegisterGeneralMessageHandler(HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM)) noexcept
		{
			generalHandler = std::bind_front(memberFunction, std::bit_cast<T*>(this));
		}
		template <typename T>
		void RegisterGeneralHandler(HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM) const) noexcept
		{
			generalHandler = std::bind_front(memberFunction, std::bit_cast<const T*>(this));
		}
		void RemoveGeneralHandler();

		virtual auto OnDPIChange(float dpiScale, RectI suggestedRect) -> HandlerResult;
		virtual void AdjustChildWindowsForDPI(float dpiScale);
		virtual void OnChildAdded(WindowPtr<Window> window);
		virtual void OnChildRemoved();

		private:
		HWND hWnd = nullptr;
		HWND parenthWnd = nullptr;
		ChildWindowList childWindows;

		UINT prevDpi = DEFAULT_SCREEN_DPI;

		HandlerMap handlerMap;
		std::optional<Handler> generalHandler;
		TimerMap timerMap;
		WindowClass::WindowClassPtr windowClass;
		
		auto OnDPIChanged(UINT msg, WPARAM wParam, LPARAM lParam) -> HandlerResult;
	};

	auto _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
}
EnableEnumFlag(PGUI::Core::HandlerResultFlag);
