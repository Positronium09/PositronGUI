#pragma once

#include "ui/Control.hpp"
#include "core/Event.hpp"
#include "helpers/StringHashes.hpp"
#include <vector>
#include <unordered_map>


namespace PGUI::UI::Controls
{
	using RadioButtonGroupMap = std::unordered_map<
		std::wstring,
		std::vector<PGUI::Core::Event<bool>>,
		WStringHash,
		std::equal_to<>>;

	class RadioButton : public Control
	{
		public:
		RadioButton();

		private:
		RadioButtonGroupMap groups;
	};
}
