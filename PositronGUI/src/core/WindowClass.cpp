#include "core/WindowClass.hpp"

#include "core/Window.hpp"


namespace PGUI::Core
{
	template<typename ...Arg> std::shared_ptr<WindowClass> static CreateWindowClassSharedPtr(Arg&&...arg)
	{
		struct EnableMakeShared : public WindowClass
		{
			explicit(false) EnableMakeShared(Arg&&...arg) : WindowClass(arg...) { }
		};
		return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);
	}
	

	WindowClass::WindowClass(std::wstring_view _className, UINT style, 
		HBRUSH backgroundBrush, HICON icon, HCURSOR cursor, HICON smIcon) :
		className(_className)
	{
		WNDCLASSEXW wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = style;
		wc.lpfnWndProc = _WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetHInstance();
		wc.hbrBackground = backgroundBrush;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = _className.data();
		wc.hIcon = icon ? icon : static_cast<HICON>(LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, NULL, NULL, LR_SHARED | LR_DEFAULTSIZE));
		wc.hCursor = cursor ? cursor : static_cast<HCURSOR>(LoadImageW(nullptr, IDC_ARROW, IMAGE_CURSOR, NULL, NULL, LR_SHARED | LR_DEFAULTSIZE));
		wc.hIconSm = smIcon ? smIcon : static_cast<HICON>(LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, NULL, NULL, LR_SHARED | LR_DEFAULTSIZE));

		classAtom = RegisterClassExW(&wc);

		if (classAtom == NULL)
		{
			throw ErrorHandling::Win32Exception{ };
		}
	}

	WindowClass::WindowClassPtr WindowClass::Create(std::wstring_view className, UINT style,
		HBRUSH backgroundBrush, HICON icon, HCURSOR cursor, HICON smIcon) noexcept
	{
		WNDCLASSEXW w{ };
		auto atom = static_cast<ATOM>(GetClassInfoExW(GetHInstance(), className.data(), &w));

		if (!atom)
		{
			std::shared_ptr<WindowClass> windowClass = CreateWindowClassSharedPtr(
				className, style, backgroundBrush, 
				icon, cursor, smIcon);

			atom = windowClass->Atom();
			registeredClasses[atom] = windowClass;
		}

		return registeredClasses[atom];
	}

	WindowClass::~WindowClass() noexcept
	{
		UnregisterClassW(className.c_str(), GetHInstance());
	}

	std::wstring_view WindowClass::ClassName() const noexcept
	{
		return className;
	}

	ATOM WindowClass::Atom() const noexcept
	{
		return classAtom;
	}
}
