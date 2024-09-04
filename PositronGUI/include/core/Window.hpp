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
	//EnableEnumFlag(HandlerResultFlag);

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
	concept WindowType = std::is_same_v<T, Window> || std::derived_from<T, Window>;

	template<WindowType T>
	using WindowOwnPtr = std::unique_ptr<T>;
	template<WindowType T>
	using WindowPtr = T*;

	using ChildWindowList = std::vector<WindowOwnPtr<Window>>;

	using TimerId = UINT_PTR;
	using TimerCallback = std::function<void(TimerId)>;
	using TimerMap = std::unordered_map<TimerId, TimerCallback>;

	[[nodiscard]] WindowPtr<Window> GetWindowFromHwnd(HWND hWnd) noexcept;

	class Window
	{
		friend LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		public:
		template <std::derived_from<Window> T, typename ...Args>
		[[nodiscard]] static WindowOwnPtr<T> Create(const WindowCreateParams& createParams, Args&&... args)
		{
			auto window = std::make_unique<T>(std::forward<Args>(args)...);
			auto wnd = window.get();

			auto size = AdjustForDPI(SizeF{ createParams.size }, static_cast<float>(GetDpiForSystem()));
			RECT rc = RectI{ PointI{ }, size };
			AdjustWindowRectExForDpi(&rc, createParams.style, FALSE, createParams.exStyle, GetDpiForSystem());
			RectI rect = rc;
			auto scaledSize = rect.Size();

			CreateWindowExW(createParams.exStyle, 
				window->windowClass->GetClassName().data(), createParams.windowName.data(),
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
		template <std::derived_from<Window> T, typename ...Args>
		WindowPtr<T> AddChildWindow(const WindowCreateParams& createParams, Args&&... args)
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
				window->windowClass->GetClassName().data(), createParams.windowName.data(),
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

			return wnd;
		}
		template <std::derived_from<Window> T>
		WindowPtr<T> AddChildWindow(const WindowOwnPtr<T> window)
		{
			auto wnd = window.get();

			auto style = GetWindowLongPtrW(wnd->Hwnd(), GWL_STYLE);
			SetWindowLongPtrW(wnd->Hwnd(), GWL_STYLE, static_cast<LONG_PTR>(style | WS_CHILD));

			SetParent(wnd->Hwnd(), Hwnd());

			childWindows.push_back(std::move(window));

			return wnd;
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
		[[nodiscard]] bool IsVisible() const noexcept;

		void Move(PointL newPos) const noexcept;
		void Resize(SizeL newSize) const noexcept;
		void MoveAndResize(RectL newRect) const noexcept;
		void MoveAndResize(PointL newPos, SizeL newSize) const noexcept;

		[[nodiscard]] UINT GetDPI() const noexcept;
		[[nodiscard]] D2D1_MATRIX_3X2_F GetDpiScaleTransform(std::optional<PointF> center = std::nullopt) const noexcept;

		[[nodiscard]] std::span<PointL> MapPoints(HWND hWndTo, std::span<PointL> points) const noexcept;
		[[nodiscard]] PointL MapPoint(HWND hWndTo, PointL point) const noexcept;

		[[nodiscard]] std::span<RectL> MapRects(HWND hWndTo, std::span<RectL> rects) const noexcept;
		[[nodiscard]] RectL MapRect(HWND hWndTo, RectL rect) const noexcept;

		[[nodiscard]] WindowPtr<Window> ChildWindowFromPoint(PointL point, UINT flags) const noexcept;

		template <typename T>
		[[nodiscard]] T ScaleByDPI(T val) const noexcept
		{
			return static_cast<T>(val * 
				static_cast<float>(GetDPI()) / Core::DEFAULT_SCREEN_DPI);
		}
		template <typename T>
		[[nodiscard]] T UnScaleByDPI(T val) const noexcept
		{
			return static_cast<T>(val *
				Core::DEFAULT_SCREEN_DPI / static_cast<float>(GetDPI()));
		}

		TimerId AddTimer(TimerId id, std::chrono::milliseconds delay, 
			std::optional<TimerCallback> callback = std::nullopt) noexcept;
		void RemoveTimer(TimerId id) noexcept;
		bool HasTimer(TimerId id) const noexcept;

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
		[[nodiscard]] RectL GetClientRectWithoutDPI() const noexcept;
		[[nodiscard]] SizeL GetWindowSize() const noexcept;
		[[nodiscard]] SizeL GetClientSize() const noexcept;
		[[nodiscard]] SizeL GetClientSizeWithoutDPI() const noexcept;

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

		virtual HandlerResult OnDPIChange(float dpiScale, RectI suggestedRect);
		virtual void AdjustChildWindowsForDPI(float dpiScale);

		private:
		HWND hWnd = nullptr;
		HWND parenthWnd = nullptr;
		ChildWindowList childWindows;

		UINT prevDpi = DEFAULT_SCREEN_DPI;

		HandlerMap handlerMap;
		std::optional<Handler> generalHandler;
		TimerMap timerMap;
		WindowClass::WindowClassPtr windowClass;
		
		HandlerResult OnDPIChanged(UINT msg, WPARAM wParam, LPARAM lParam);
	};

	LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
template<> struct enum_flag_detail::enum_flag_enable<PGUI::Core::HandlerResultFlag> : public std::true_type { };
