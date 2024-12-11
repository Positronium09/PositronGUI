#pragma once

#include <functional>
#include <list>


namespace PGUI::Core
{
	template <typename... Args>
	class Event
	{
		using EventHandler = std::function<void(Args...)>;

		public:
		void Subscribe(const EventHandler& handler) noexcept
		{
			eventHandlers.push_back(handler);
		}

		void Emit(Args... args) const
		{
			for (auto& handler : eventHandlers)
			{
				std::invoke(handler, args...);
			}
		}

		private:
		std::list<EventHandler> eventHandlers;
	};

	template <>
	class Event<void>
	{
		using EventHandler = std::function<void(void)>;

		public:
		void Subscribe(const EventHandler& handler)
		{
			eventHandlers.push_back(handler);
		}

		void Emit() const
		{
			for (const auto& handler : eventHandlers)
			{
				std::invoke(handler);
			}
		}

		private:
		std::list<EventHandler> eventHandlers;
	};
}
