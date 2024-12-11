#pragma once


#include <string>
#include <stdexcept>
#include <Windows.h>


namespace PGUI::Core
{
	class PGUIException : public std::exception
	{
		public:
		explicit PGUIException(HRESULT errorCode) noexcept;

		[[nodiscard]] auto GetErrorCode() const noexcept { return errorCode; }
		[[nodiscard]] auto GetErrorMessage() const noexcept -> std::wstring_view { return errorMessage; }

		private:
		HRESULT errorCode;
		std::wstring errorMessage;
	};

	class Win32Exception : public PGUIException
	{
		public:
		Win32Exception() noexcept;
		explicit Win32Exception(DWORD errorCode) noexcept;
	};

	class HresultException : public PGUIException
	{
		public:
		explicit HresultException(HRESULT hResult) noexcept;
	};
}
