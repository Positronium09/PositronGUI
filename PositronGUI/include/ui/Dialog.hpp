#pragma once

#include "core/DirectCompositionWindow.hpp"
#include "core/MessageLoop.hpp"


namespace PGUI::UI
{
	struct DialogCreateParams
	{
		HWND parentHwnd;
		DWORD style;

		DialogCreateParams(HWND _parentHwnd, DWORD _style = NULL) noexcept :
			parentHwnd(_parentHwnd), style(_style)
		{
		}
	};

	//? Note that dialog boxes are different from the win32 dialog boxes
	//? WM_INITDIALOG is sent just for completeness return value is ignored
	class Dialog : public Core::DirectCompositionWindow
	{
		public:
		explicit Dialog(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;

		template <std::derived_from<Dialog> T, typename ...Args>
		static Core::WindowPtr<T> Create(const DialogCreateParams& dialogParam,
			const Core::WindowCreateParams& createParams, Args... args)
		{
			DWORD style = dialogParam.style | WS_CAPTION | WS_SYSMENU;
			DWORD exStyle = createParams.exStyle | WS_EX_NOREDIRECTIONBITMAP;

			if (dialogParam.style & DS_MODALFRAME)
			{
				exStyle |= WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE;
			}
			if (dialogParam.style & DS_CONTEXTHELP)
			{
				exStyle |= WS_EX_CONTEXTHELP;
			}
			if (dialogParam.style & DS_CONTROL)
			{
				style &= ~(WS_CAPTION | WS_SYSMENU);
				exStyle |= WS_EX_CONTROLPARENT;
			}

			auto dialogRect = RectI{
				createParams.position.x, createParams.position.y,
				createParams.position.x + createParams.size.cx, createParams.position.y + createParams.size.cy
			};

			dialogRect.CenterAround(Core::GetWindowFromHwnd(dialogParam.parentHwnd)->GetWindowRect().Center());

			const auto position = dialogRect.TopLeft();
			const auto size = dialogRect.Size();

			auto window = std::make_shared<T>(args...);

			CreateWindowExW(exStyle,
				window->GetWindowClass()->ClassName().data(), createParams.windowName.data(),
				style,
				position.x, position.y,
				size.cx, size.cy,
				dialogParam.parentHwnd, NULL, GetHInstance(),
				static_cast<LPVOID>(window.get()));

			if (window->Hwnd() == NULL)
			{
				Core::ErrorHandling::Logger::Error(L"CreateWindow failed in Dialog::Create");
				auto errorCode = GetLastError();
				Core::ErrorHandling::Logger::Error(std::format(L"Code: {}", errorCode));
				Core::ErrorHandling::Logger::Error(GetWin32ErrorMessage(errorCode));
				throw Core::ErrorHandling::Win32Exception{ };
			}

			SendMessageW(window->Hwnd(), WM_INITDIALOG, 
				std::bit_cast<WPARAM>(window->Hwnd()), 
				std::bit_cast<LPARAM>(window.get()));

			return window;
		}

		void CenterAroundParent() const noexcept;

		private:
		Core::HandlerResult OnInitDialog(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	};
	using ModalessDialog = Dialog;

	class ModalDialog : public Dialog
	{
		public:
		explicit ModalDialog(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;

		virtual int RunModal() noexcept;
		
		protected:
		[[nodiscard]] bool IsRunning() const noexcept;
		void SetRunning(bool running) noexcept;

		private:
		bool running = false;
		Core::HandlerResult OnClose(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};

	class ModalMessageLoop : public Core::MessageLoopBase
	{
		public:
		explicit ModalMessageLoop(HWND modalDialogHwnd) noexcept;
		int Run() noexcept override;

		private:
		HWND modalDialogHwnd;
	};
}
