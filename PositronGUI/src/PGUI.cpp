#include "PGUI.hpp"
#include "core/Exceptions.hpp"

#include <windows.foundation.h>


namespace PGUI
{
	void Initialize()
	{
		InitializeWindowsFoundation();
		
		BOOL suoParam = TRUE;
		if (BOOL succeeded =
			SetUserObjectInformationW(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &suoParam, sizeof(suoParam));
			succeeded == 0)
		{
			HR_L(HRESULT_FROM_WIN32(GetLastError()));
			throw Core::ErrorHandling::Win32Exception{ };
		}

	}
	void InitializeWindowsFoundation()
	{
		HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED); HR_T(hr);
	}

	void UninitializeWindowsFoundation()
	{
		Windows::Foundation::Uninitialize();
	}
}
