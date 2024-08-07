#pragma once

#include "core/Logger.hpp"
#include "helpers/HelperFunctions.hpp"

#include <chrono>
#include <source_location>


namespace PGUI
{
	class ScopedTimer
	{
		public:
		ScopedTimer(std::source_location location = std::source_location::current()) noexcept : 
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
				std::format("{} {} {}", location.file_name(), location.function_name(),
				std::chrono::duration_cast<std::chrono::milliseconds>(elapsed)))
			);
		}
		private:
		std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
		std::source_location location;
	};
}
