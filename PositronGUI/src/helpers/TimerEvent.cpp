#include "helpers/TimerEvent.hpp"

#include <functional>


namespace PGUI
{
	TimerEvent::TimerEvent(milliseconds interval, std::optional<milliseconds> startDelay) noexcept :
		thread{ std::bind_front(&TimerEvent::ThreadFunction, this) },
		interval{ interval }, startDelay{ startDelay.value_or(interval) }
	{
	}

	void TimerEvent::ThreadFunction(std::stop_token stopToken) const
	{
		using high_res_clock = std::chrono::high_resolution_clock;

		using namespace std::chrono_literals;

		auto start = high_res_clock::now();
		auto end = start + interval;
		do
		{
			std::this_thread::yield();
		} while (std::chrono::high_resolution_clock::now() < end && !stopToken.stop_requested());

		std::this_thread::sleep_for(startDelay);

		while (!stopToken.stop_requested())
		{
			Emit();
			
			start = high_res_clock::now();
			end = start + interval;
			do
			{
				std::this_thread::yield();
			} while (std::chrono::high_resolution_clock::now() < end);
		}
	}
}
