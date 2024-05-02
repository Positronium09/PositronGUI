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
		explicit PGUIException(HRESULT errorCode) noexcept;

		~PGUIException() noexcept override = default;

		[[nodiscard]] HRESULT GetErrorCode() const noexcept;
		[[nodiscard]] const std::wstring& GetErrorMessage() const noexcept;

		char const* what() const override;

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

namespace PGUI
{
	static inline void HR_T(HRESULT hr) noexcept(false)
	{
		if (FAILED(hr))
		{
			throw Core::ErrorHandling::HresultException{ hr };
		}
	}
}
