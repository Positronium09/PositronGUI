#pragma once

#include "Logger.hpp"
#include "Exceptions.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Size.hpp"
#include "WindowClass.hpp"
#include "helpers/HelperFunctions.hpp"
#include "helpers/EnumOperators.hpp"

#include <bit>
#include <concepts>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <Windows.h>


namespace PGUI::Core
{
	enum class HandlerResultFlags
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
		HandlerResultFlags flags;

		HandlerResult(LRESULT _result, HandlerResultFlags _flags = HandlerResultFlags::Nothing) :
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
	using ChildWindowList = std::vector<std::unique_ptr<Window>>;

	Window* GetWindowFromHwnd(HWND hWnd);

	class Window
	{
		friend LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		public:
		template <std::derived_from<Window> T, typename ...Args>
		static std::unique_ptr<T> Create(const WindowCreateParams& createParams, Args... args)
		{
			auto window = std::make_unique<T>(args...);

			CreateWindowExW(createParams.exStyle, 
				window->windowClass->ClassName().data(), createParams.windowName.data(),
				createParams.style,
				createParams.position.x, createParams.position.x, 
				createParams.size.cx, createParams.size.cy,
				NULL, NULL, GetHInstance(),
				static_cast<LPVOID>(window.get()));

			if (window->hWnd == NULL)
			{
				throw ErrorHandling::Win32Exception{ };
			}

			return window;
		}

		explicit Window(const WindowClass::WindowClassPtr& wndClass);
		virtual ~Window();
		
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		Window& operator=(Window&) noexcept = delete;
		
		explicit(false) operator HWND() const { return hWnd; }
		[[nodiscard]] HWND Hwnd() const { return hWnd; }

		void Show(int show=SW_SHOW);

		void Move(PointL newPos) const;
		void Resize(SizeL newSize) const;

		[[nodiscard]] PointL ScreenToClient(PointL point) const;
		[[nodiscard]] RectL ScreenToClient(RectL rect) const;

		[[nodiscard]] PointL ClientToScreen(PointL point) const;
		[[nodiscard]] RectL ClientToScreen(RectL rect) const;

		[[nodiscard]] const ChildWindowList& GetChildWindowList() const;

		[[nodiscard]] RectL GetWindowRect() const;
		[[nodiscard]] RectL GetClientRect() const;
		[[nodiscard]] SizeL GetWindowSize() const;
		[[nodiscard]] SizeL GetClientSize() const;

		void Invalidate() const;

		protected:
		virtual void RegisterMessageHandler(UINT msg, const Handler& handler) final;
		template <typename T>
		void RegisterMessageHandler(UINT msg, HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM))
		{
			RegisterMessageHandler(msg, std::bind_front(memberFunction, std::bit_cast<T*>(this)));
		}
		template <typename T>
		void RegisterMessageHandler(UINT msg, HandlerResult(T::* memberFunction)(UINT, WPARAM, LPARAM) const)
		{
			RegisterMessageHandler(msg, std::bind_front(memberFunction, std::bit_cast<const T*>(this)));
		}

		private:
		HWND hWnd = nullptr;
		HWND parenthWnd = nullptr;
		ChildWindowList childWindows;

		HandlerMap handlerMap;
		WindowClass::WindowClassPtr windowClass;
		public:

		bool operator==(const Window& other) const;
	};

	LRESULT _WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
