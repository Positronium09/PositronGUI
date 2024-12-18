#include "core/Exceptions.hpp"

#include "helpers/HelperFunctions.hpp"


namespace PGUI::Core
{
	PGUIException::PGUIException(HRESULT _errorCode) noexcept :
		std::exception{ WStringToString(GetHresultErrorMessage(_errorCode)).c_str() },
		errorCode(_errorCode), errorMessage(GetHresultErrorMessage(_errorCode))
	{
	}

	Win32Exception::Win32Exception() noexcept :
		PGUIException{ HresultFromWin32() }
	{
	}

	Win32Exception::Win32Exception(DWORD errorCode) noexcept :
		PGUIException{ HresultFromWin32(errorCode) }
	{
	}

	HresultException::HresultException(HRESULT hResult) noexcept :
		PGUIException{ hResult }
	{
	}
}
