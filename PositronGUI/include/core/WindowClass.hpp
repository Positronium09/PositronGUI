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

		static WindowClassPtr Create(std::wstring_view _className,
			UINT style = CS_HREDRAW | CS_VREDRAW, HBRUSH backgroundBrush = nullptr,
			HICON icon = nullptr, HCURSOR cursor = nullptr, HICON smIcon = nullptr) noexcept;

		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		WindowClass(WindowClass&&) noexcept = delete;
		WindowClass& operator=(WindowClass&) noexcept = delete;

		~WindowClass() noexcept;

		[[nodiscard]] std::wstring_view ClassName() const noexcept;
		[[nodiscard]] ATOM Atom() const noexcept;

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
