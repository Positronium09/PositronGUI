#pragma once

#include "helpers/HelperFunctions.hpp"

#include <memory>
#include <string>

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

	std::string GetLogLevelStr(LogLevel logLevel);

	class ILogger
	{
		public:
		virtual ~ILogger() = default;

		virtual void Log(LogLevel logLevel, std::string_view string) = 0;
	};

	class Logger final
	{
		public:
		static void LogStackTrace();
		static void Log(LogLevel logLevel, std::string_view string);
		static void Log(std::string_view string);

		static void Trace(std::string_view string);
		static void Debug(std::string_view string);
		static void Info(std::string_view string);
		static void Warning(std::string_view string);
		static void Error(std::string_view string);
		static void Fatal(std::string_view string);

		static void SetLogger(ILogger* logger);
		[[nodiscard]] static ILogger* GetLogger();

		static void SetLogLevel(LogLevel logLevel);
		[[nodiscard]] static LogLevel GetLogLevel();

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
		void Log(LogLevel logLevel, std::string_view string) override;
	};
}

namespace PGUI
{
	static inline void HR_L(HRESULT hr)
	{
		if (FAILED(hr))
		{
			Core::ErrorHandling::Logger::Error(GetHresultErrorMessage(hr));
		}
	}
}
