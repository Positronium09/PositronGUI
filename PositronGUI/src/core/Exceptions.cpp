#include "core/Exceptions.hpp"

#include "helpers/HelperFunctions.hpp"


namespace PGUI::Core::ErrorHandling
{
	ErrorHandling::PGUIException::PGUIException(HRESULT _errorCode) noexcept :
		errorCode(_errorCode), errorMessage(GetHresultErrorMessage(_errorCode))
	{
	}
	
	HRESULT ErrorHandling::PGUIException::GetErrorCode() const noexcept
	{
		return errorCode;
	}
	const std::wstring& ErrorHandling::PGUIException::GetErrorMessage() const noexcept
	{
		return errorMessage;
	}

	char const* PGUIException::what() const
	{
		return WStringToString(errorMessage).c_str();
	}

	ErrorHandling::Win32Exception::Win32Exception() noexcept :
		PGUIException{ HRESULT_FROM_WIN32(GetLastError()) }
	{
	}

	ErrorHandling::Win32Exception::Win32Exception(DWORD errorCode) noexcept :
		PGUIException{ HRESULT_FROM_WIN32(errorCode) }
	{
	}

	ErrorHandling::HresultException::HresultException(HRESULT hResult) noexcept :
		PGUIException{ hResult }
	{
	}
}
