#include "core/Logger.hpp"

#include <stacktrace>
#include <system_error>
#include <Windows.h>

#undef ERROR


namespace PGUI::Core::ErrorHandling
{
	std::string GetLogLevelStr(LogLevel logLevel)
	{
		switch (logLevel)
		{
			using enum PGUI::Core::ErrorHandling::LogLevel;

			case DEBUG:
				return "DEBUG";
			case TRACE:
				return "TRACE";
			case INFO:
				return "INFO";
			case WARNING:
				return "WARNING";
			case ERROR:
				return "ERROR";
			case FATAL:
				return "FATAL";
		}
		std::unreachable();
	}

	void Logger::LogStackTrace()
	{
		for (auto& entry : std::stacktrace::current())
		{
			logger->Log(LogLevel::TRACE, std::to_string(entry) + "\n");
		}
	}

	void Logger::Log(LogLevel _logLevel, std::string_view string)
	{
		logger->Log(_logLevel, string);
	}

	void Logger::Log(std::string_view string)
	{
		Log(logLevel, string);
	}

	void Logger::Trace(std::string_view string)
	{
		Log(LogLevel::TRACE, string);
	}
	void Logger::Debug(std::string_view string)
	{
		Log(LogLevel::DEBUG, string);
	}
	void Logger::Info(std::string_view string)
	{
		Log(LogLevel::INFO, string);
	}
	void Logger::Warning(std::string_view string)
	{
		Log(LogLevel::WARNING, string);
	}
	void Logger::Error(std::string_view string)
	{
		Log(LogLevel::ERROR, string);
	}
	void Logger::Fatal(std::string_view string)
	{
		Log(LogLevel::FATAL, string);
	}

	void Logger::SetLogger(ILogger* _logger)
	{
		logger = _logger;
	}

	ILogger* Logger::GetLogger()
	{
		return logger;
	}

	void Logger::SetLogLevel(LogLevel _logLevel)
	{
		logLevel = _logLevel;
	}
	LogLevel Logger::GetLogLevel()
	{
		return logLevel;
	}

	void DebugConsoleLogger::Log(LogLevel logLevel, std::string_view string)
	{
		OutputDebugStringA("[");
		OutputDebugStringA(GetLogLevelStr(logLevel).c_str());
		OutputDebugStringA("] ");
		OutputDebugStringA(string.data());
		OutputDebugStringA("\n");
	}
}
