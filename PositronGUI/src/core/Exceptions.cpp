#include "core/Exceptions.hpp"

#include "helpers/HelperFunctions.hpp"


namespace PGUI::Core::ErrorHandling
{
	ErrorHandling::PGUIException::PGUIException(HRESULT _errorCode) : 
		errorCode(_errorCode), errorMessage(GetHresultErrorMessage(_errorCode))
	{
	}
	
	HRESULT ErrorHandling::PGUIException::GetErrorCode() const
	{
		return errorCode;
	}

	const char* ErrorHandling::PGUIException::what() const
	{
		return errorMessage.c_str();
	}

	ErrorHandling::Win32Exception::Win32Exception() : 
		PGUIException{ HRESULT_FROM_WIN32(GetLastError()) }
	{
	}

	ErrorHandling::Win32Exception::Win32Exception(DWORD errorCode) : 
		PGUIException{ HRESULT_FROM_WIN32(errorCode) }
	{
	}

	ErrorHandling::HresultException::HresultException(HRESULT hResult) : 
		PGUIException{ hResult }
	{
	}
}
