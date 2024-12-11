#include "ui/Dialog.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI
{
	using namespace PGUI::Core;

	Dialog::Dialog(const WindowClass::WindowClassPtr& wndClass) noexcept :
		DirectCompositionWindow{ wndClass }
	{
		RegisterMessageHandler(WM_INITDIALOG, &Dialog::OnInitDialog);
	}

	void Dialog::CenterAroundParent() const noexcept
	{
		auto dialogRect = GetClientRect();

		dialogRect.CenterAround(Core::GetWindowFromHwnd(ParentHwnd())->GetWindowRect().Center());

		Move(dialogRect.TopLeft());
	}

	auto Dialog::OnInitDialog(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> HandlerResult
	{
		return { NULL, HandlerResultFlag::ForceThisResult };
	}

	ModalDialog::ModalDialog(const WindowClass::WindowClassPtr& wndClass) noexcept :
		Dialog{ wndClass }
	{
		RegisterMessageHandler(WM_CLOSE, &ModalDialog::OnClose);
	}

	auto ModalDialog::RunModal() noexcept -> int
	{
		Show();
		MSG msg{ };

		if (auto* parentHwnd = ParentHwnd();
			parentHwnd != nullptr)
		{
			GetWindowFromHwnd(parentHwnd)->Enable(false);
		}
		running = true;		

		BOOL ret = 1;
		while (running && ret != 0)
		{
			GetMessageW(&msg, nullptr, 0, 0);
			if (ret == -1)
			{
				auto errCode = GetLastError();
				HR_L(HresultFromWin32(errCode));
				return errCode;
			}
			
			
				if (!IsDialogMessageW(Hwnd(), &msg))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
		
		}

		if (auto* parentHwnd = ParentHwnd();
			parentHwnd != nullptr)
		{
			SetForegroundWindow(parentHwnd);
			GetWindowFromHwnd(parentHwnd)->Enable(true);
		}

		return static_cast<int>(msg.wParam);
	}

	void ModalDialog::SetRunning(bool _running) noexcept
	{
		running = _running;
	}

	auto ModalDialog::OnClose(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		running = false;
		return 0;
	}

	ModalMessageLoop::ModalMessageLoop(HWND _modalDialogHwnd) noexcept :
		modalDialogHwnd(_modalDialogHwnd)
	{
	}
	auto ModalMessageLoop::Run() noexcept -> int
	{
		MSG msg{ };

		BOOL ret = 0;
		while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0)
		{
			if (ret == -1)
			{
				auto errCode = GetLastError();
				HR_L(HresultFromWin32(errCode));
				return static_cast<int>(errCode);
			}
			
			
				if (!IsDialogMessageW(modalDialogHwnd, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
		
		}

		return static_cast<int>(msg.wParam);
	}
}
