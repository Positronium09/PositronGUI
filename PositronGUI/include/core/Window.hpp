#pragma once

#include "Logger.hpp"
#include "Exceptions.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Size.hpp"
#include "WindowClass.hpp"
#include "helpers/HelperFunctions.hpp"
#include "helpers/Enum.hpp"

#include <bit>
#include <ranges>
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
	using HandlerResultFlag = Enum<_handler_result_flag_values>;

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


	[[nodiscard]] Window* GetWindowFromHwnd(HWND hWnd) noexcept;

	class Window
	{
		friend LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		public:
		template <std::derived_from<Window> T, typename ...Args>
		static WindowPtr<T> Create(const WindowCreateParams& createParams, Args... args)
		{
			auto window = std::make_shared<T>(args...);

			CreateWindowExW(createParams.exStyle, 
				window->windowClass->ClassName().data(), createParams.windowName.data(),
				createParams.style,
				createParams.position.x, createParams.position.y, 
				createParams.size.cx, createParams.size.cy,
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

			return window;
		}
		template <std::derived_from<Window> T, typename ...Args>
		WindowPtr<T> AddChildWindow(const WindowCreateParams& createParams, Args... args)
		{
			auto window = std::make_shared<T>(args...);

			CreateWindowExW(createParams.exStyle,
				window->windowClass->ClassName().data(), createParams.windowName.data(),
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

			return window;
		}
		template <std::derived_from<Window> T>
		WindowPtr<T> AddChildWindow(const WindowPtr<T> window)
		{
			auto style = GetWindowLongPtrW(window->Hwnd(), GWL_STYLE);
			SetWindowLongPtrW(window->Hwnd(), GWL_STYLE, static_cast<LONG_PTR>(style | WS_CHILD));

			SetParent(window->Hwnd(), Hwnd());

			childWindows.push_back(window);

			return window;
		}

		void RemoveChildWindow(HWND childHwnd)
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

		explicit Window(const WindowClass::WindowClassPtr& wndClass) noexcept;
		virtual ~Window() noexcept;
		
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(Window&) noexcept = delete;
		
		explicit(false) operator HWND() const noexcept { return hWnd; }
		[[nodiscard]] HWND Hwnd() const noexcept { return hWnd; }
		[[nodiscard]] HWND ParentHwnd() const noexcept { return parenthWnd; }

		void Show(int show=SW_SHOW) noexcept;

		void Move(PointL newPos) const noexcept;
		void Resize(SizeL newSize) const noexcept;

		[[nodiscard]] PointL ScreenToClient(PointL point) const noexcept;
		[[nodiscard]] RectL ScreenToClient(RectL rect) const noexcept;

		[[nodiscard]] PointL ClientToScreen(PointL point) const noexcept;
		[[nodiscard]] RectL ClientToScreen(RectL rect) const noexcept;

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

		private:
		HWND hWnd = nullptr;
		HWND parenthWnd = nullptr;
		ChildWindowList childWindows;

		HandlerMap handlerMap;
		WindowClass::WindowClassPtr windowClass;
	};

	LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
