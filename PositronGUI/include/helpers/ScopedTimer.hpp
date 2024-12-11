#pragma once

#include "core/Logger.hpp"
#include "helpers/HelperFunctions.hpp"

#include <shlwapi.h>
#include <chrono>
#include <source_location>


namespace PGUI
{
	class ScopedTimer
	{
		public:
		explicit ScopedTimer(std::wstring_view name, std::source_location location = std::source_location::current()) :
			name{ name }, location{ location }
		{
		}
		explicit ScopedTimer(std::source_location location = std::source_location::current()) noexcept : 
			location{ location }
		{
		}
		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer(ScopedTimer&&) = delete;
		~ScopedTimer()
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = end - start;
			Core::Logger::Debug(StringToWString(
				std::format("{} {} {} {}", 
					WStringToString(name),
					std::chrono::duration_cast<std::chrono::milliseconds>(elapsed), 
					PathFindFileNameA(location.file_name()), location.function_name())
			));
		}
		private:
		std::wstring name;
		std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
		std::source_location location;
	};
}
