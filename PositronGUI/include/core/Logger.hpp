#pragma once

#include "helpers/HelperFunctions.hpp"
#include "Exceptions.hpp"

#include <format>
#include <memory>
#include <string>
#include <stacktrace>

#undef ERROR


namespace PGUI::Core::ErrorHandling
{
	enum class LogLevel
	{
		TRACE = 0,
		DEBUG = 1,
		INFO = 2,
		WARNING = 3,
		ERROR = 4,
		FATAL = 5
	};

	constexpr [[nodiscard]] std::wstring GetLogLevelStr(LogLevel logLevel) noexcept;

	class ILogger
	{
		public:
		virtual ~ILogger() noexcept = default;

		virtual void Log(LogLevel logLevel, std::wstring_view string) noexcept = 0;
	};

	class Logger final
	{
		public:
		static void LogStackTrace() noexcept;
		static void Log(LogLevel logLevel, std::wstring_view string) noexcept;
		static void Log(std::wstring_view string) noexcept;

		static void Trace(std::wstring_view string) noexcept;
		static void Debug(std::wstring_view string) noexcept;
		static void Info(std::wstring_view string) noexcept;
		static void Warning(std::wstring_view string) noexcept;
		static void Error(std::wstring_view string) noexcept;
		static void Fatal(std::wstring_view string) noexcept;

		static void SetLogger(ILogger* logger) noexcept;
		static [[nodiscard]] ILogger* GetLogger() noexcept;

		static void SetLogLevel(LogLevel logLevel) noexcept;
		static [[nodiscard]] LogLevel GetLogLevel() noexcept;

		private:
		static inline ILogger* logger;
		static inline LogLevel logLevel =
		#ifdef _DEBUG
			LogLevel::DEBUG;
		#else
			LogLevel::INFO;
		#endif
	};

	class DebugConsoleLogger : public ILogger
	{
		public:
		void Log(LogLevel logLevel, std::wstring_view string) noexcept override;
	};
}

namespace PGUI
{
	static inline void HR_L(HRESULT hr) noexcept
	{
		if (FAILED(hr))
		{
			std::stacktrace trace = std::stacktrace::current();

			for (size_t i = 1; i < trace.size(); i++)
			{
				const auto& traceEntry = trace.at(i);

				Core::ErrorHandling::Logger::Error(std::format(
					L"Line {} in {}\n", traceEntry.source_line(), StringToWString(traceEntry.source_file())
				));
			}

			Core::ErrorHandling::Logger::Error(GetHresultErrorMessage(hr));
		}
	}

	static inline void HR_L(const Core::ErrorHandling::HresultException& hrException) noexcept
	{
		HR_L(hrException.GetErrorCode());
	}
}
