#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <Windows.h>


namespace PGUI::Core
{
	class WindowClass
	{
		public:
		using WindowClassPtr = std::shared_ptr<WindowClass>;

		static auto Create(std::wstring_view _className,
			UINT style = CS_HREDRAW | CS_VREDRAW, HBRUSH backgroundBrush = nullptr,
			HICON icon = nullptr, HCURSOR cursor = nullptr, HICON smIcon = nullptr) noexcept -> WindowClassPtr;

		WindowClass(const WindowClass&) = delete;
		auto operator=(const WindowClass&) -> WindowClass& = delete;
		WindowClass(WindowClass&&) noexcept = delete;
		auto operator=(WindowClass&&) noexcept -> WindowClass& = delete;

		~WindowClass() noexcept;

		[[nodiscard]] auto ClassName() const noexcept -> std::wstring_view { return className; }
		[[nodiscard]] auto GetAtom() const noexcept -> ATOM { return classAtom; }

		protected:
		WindowClass(std::wstring_view _className,
			UINT style, HBRUSH backgroundBrush,
			HICON icon, HCURSOR cursor, HICON smIcon);

		private:
		std::wstring className;
		ATOM classAtom;

		static inline std::unordered_map<ATOM, WindowClassPtr> registeredClasses;
	};
}
