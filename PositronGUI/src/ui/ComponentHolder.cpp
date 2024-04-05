#include "ui/ComponentHolder.hpp"


namespace PGUI::UI
{
	Core::Event<void>& ComponentHolder::ContentChangedEvent() noexcept
	{
		return contentChangedEvent;
	}
}
