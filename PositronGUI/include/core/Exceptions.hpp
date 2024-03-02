#pragma once


#include <string>
#include <stdexcept>
#include <stacktrace>
#include <Windows.h>


namespace PGUI::Core::ErrorHandling
{
	class PGUIException : public std::exception
	{
		public:
		explicit PGUIException(HRESULT errorCode);

		~PGUIException() override = default;

		[[nodiscard]] HRESULT GetErrorCode() const;

		[[nodiscard]] const char* what() const override;

		private:
		HRESULT errorCode;
		std::string errorMessage;
	};

	class Win32Exception : public PGUIException
	{
		public:
		Win32Exception();
		explicit Win32Exception(DWORD errorCode);
	};

	class HresultException : public PGUIException
	{
		public:
		explicit HresultException(HRESULT hResult);
	};
}

namespace PGUI
{
	static inline void HR_T(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw Core::ErrorHandling::HresultException{ hr };
		}
	}
}
