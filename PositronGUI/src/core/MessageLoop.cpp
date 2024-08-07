#include "core/MessageLoop.hpp"

#include "core/Exceptions.hpp"
#include "helpers/HelperFunctions.hpp"
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
				auto errCode = GetLastError();
				HR_L(HresultFromWin32(errCode));
				return errCode;
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
		throw HresultException{ E_NOTIMPL };
	}
}
