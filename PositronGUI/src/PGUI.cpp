#include "PGUI.hpp"
#include "core/Exceptions.hpp"


namespace PGUI
{
	void Initialize()
	{
		InitializeWindowsFoundation();
		
		BOOL suoParam = TRUE;
		if (BOOL succeeded =
			SetUserObjectInformationW(GetCurrentProcess(), 
				UOI_TIMERPROC_EXCEPTION_SUPPRESSION, 
				&suoParam, sizeof(suoParam));
			succeeded == 0)
		{
			HR_L(HresultFromWin32());
			throw Core::Win32Exception{ };
		}
	}

	void Uninitialize()
	{
		UninitializeWindowsFoundation();
	}

	void InitializeWindowsFoundation()
	{
		HRESULT hr = Windows::Foundation::Initialize(RO_INIT_SINGLETHREADED); HR_T(hr);
	}

	void UninitializeWindowsFoundation()
	{
		Windows::Foundation::Uninitialize();
	}
}
