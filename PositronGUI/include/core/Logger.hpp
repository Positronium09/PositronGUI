#pragma once

#include "Exceptions.hpp"

#include <memory>
#include <string>

#undef ERROR

namespace PGUI::Core
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

	[[nodiscard]] constexpr auto GetLogLevelStr(LogLevel logLevel) noexcept -> std::wstring;

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
		[[nodiscard]] static auto GetLogger() noexcept { return logger; }

		static void SetLogLevel(LogLevel logLevel) noexcept;
		[[nodiscard]] static auto GetLogLevel() noexcept { return logLevel; }

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
