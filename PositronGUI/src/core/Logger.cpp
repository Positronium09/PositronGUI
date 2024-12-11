#include "core/Logger.hpp"
#include "helpers/HelperFunctions.hpp"

#include <chrono>
#include <stacktrace>
#include <system_error>
#include <Windows.h>


namespace PGUI::Core
{
	constexpr std::wstring GetLogLevelStr(LogLevel logLevel) noexcept
	{
		switch (logLevel)
		{
			using enum PGUI::Core::LogLevel;

			case DEBUG:
				return L"DEBUG";
			case TRACE:
				return L"TRACE";
			case INFO:
				return L"INFO";
			case WARNING:
				return L"WARNING";
			case ERROR:
				return L"ERROR";
			case FATAL:
				return L"FATAL";
		}
		std::unreachable();
	}

	void Logger::LogStackTrace() noexcept
	{
		for (auto& entry : std::stacktrace::current())
		{
			logger->Log(LogLevel::TRACE, StringToWString(std::to_string(entry)));
		}
	}

	void Logger::Log(LogLevel _logLevel, std::wstring_view string) noexcept
	{
		logger->Log(_logLevel, string);
	}

	void Logger::Log(std::wstring_view string) noexcept
	{
		Log(logLevel, string);
	}

	void Logger::Trace(std::wstring_view string) noexcept
	{
		Log(LogLevel::TRACE, string);
	}
	void Logger::Debug(std::wstring_view string) noexcept
	{
		Log(LogLevel::DEBUG, string);
	}
	void Logger::Info(std::wstring_view string) noexcept
	{
		Log(LogLevel::INFO, string);
	}
	void Logger::Warning(std::wstring_view string) noexcept
	{
		Log(LogLevel::WARNING, string);
	}
	void Logger::Error(std::wstring_view string) noexcept
	{
		Log(LogLevel::ERROR, string);
	}
	void Logger::Fatal(std::wstring_view string) noexcept
	{
		Log(LogLevel::FATAL, string);
	}

	void Logger::SetLogger(ILogger* _logger) noexcept
	{
		logger = _logger;
	}

	void Logger::SetLogLevel(LogLevel _logLevel) noexcept
	{
		logLevel = _logLevel;
	}

	void DebugConsoleLogger::Log(LogLevel logLevel, std::wstring_view string) noexcept
	{
		std::chrono::zoned_time time{
			std::chrono::current_zone(),
			std::chrono::system_clock::now()
		};
		OutputDebugStringW(L"[");
		OutputDebugStringW(GetLogLevelStr(logLevel).c_str());
		OutputDebugStringW(std::format(L" {:%H-%M-%S - %d-%m-%Y}] ", time).c_str());
		OutputDebugStringW(string.data());
		OutputDebugStringW(L"\n");
	}
}
