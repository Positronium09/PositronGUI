#pragma once

#include "core/Window.hpp"
#include "core/Event.hpp"

#include <type_traits>


namespace PGUI::UI
{
	class UIComponent;

	class ComponentHolder : public Core::Window
	{
		public:
		template <std::derived_from<UIComponent> T>
		void SetContent(T* _content)
		{
			AddChildWindow<T>(_content);
			content = _content;
			OnContentChanged();
		}

		template <std::derived_from<UIComponent> T>
		[[nodiscard]] T* GetContent() const noexcept
		{
			return dynamic_cast<T>(content);
		}

		[[nodiscard]] Core::Event<void>& ContentChangedEvent() noexcept;

		protected:
		virtual void OnContentChanged() { contentChangedEvent.Emit(); }

		private:
		Core::Event<void> contentChangedEvent;

		UIComponent* content = nullptr;
	};
}
