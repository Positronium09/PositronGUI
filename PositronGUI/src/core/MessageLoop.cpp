#include "core/MessageLoop.hpp"

#include "core/Exceptions.hpp"
#include "core/Logger.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::Core
{
	int GetMessageLoop::Run() noexcept
	{
		MSG msg{ };

		BOOL ret = 0;
		while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0)
		{
			if (ret == -1)
			{
				DWORD errorCode = GetLastError();
				ErrorHandling::Logger::Error(GetWin32ErrorMessage(errorCode));
				return errorCode;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

		return static_cast<int>(msg.wParam);
	}

	//TODO Implemet
	int PeekMessageLoop::Run()
	{
		throw ErrorHandling::HresultException{ E_NOTIMPL };
	}
}
