#pragma once

#include "core/Event.hpp"
#include <chrono>
#include <optional>
#include <thread>


namespace PGUI
{
	class TimerEvent : public Core::Event<>
	{
		using milliseconds = std::chrono::milliseconds;

		public:
		TimerEvent(milliseconds interval, std::optional<milliseconds> startDelay = std::nullopt) noexcept;

		private:
		void ThreadFunction(const std::stop_token& stopToken) const;

		std::jthread thread;

		milliseconds interval;
		milliseconds startDelay;
	};
}
