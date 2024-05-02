#include "PGUI.hpp"

#include <windows.foundation.h>


namespace PGUI
{
	void InitializeWindowsFoundation()
	{
		HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED); HR_T(hr);
	}

	void UninitializeWindowsFoundation()
	{
		Windows::Foundation::Uninitialize();
	}
}
