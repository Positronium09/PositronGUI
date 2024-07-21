#include "ui/controls/TextButton.hpp"

#include "ui/Colors.hpp"

#include <algorithm>
#include <strsafe.h>

#undef min


namespace PGUI::UI::Controls
{
	TextButton::TextButtonColors TextButton::GetTextButtonColors() noexcept
	{
		TextButton::TextButtonColors colors;

		if (UIColors::IsDarkMode())
		{
			colors.normalText = Colors::Aliceblue;
			colors.normalBackground = RGBA{ 0x1b1b1b };

			colors.hoverText = Colors::Aliceblue;
			colors.hoverBackground = RGBA{ 0x202020 };

			colors.clickedText = Colors::Aliceblue;
			colors.clickedBackground = RGBA{ 0x191919 };

		}
		else
		{
			colors.normalText = Colors::Black;
			colors.normalBackground = RGBA{ 0xf3f3f3 };

			colors.hoverText = Colors::Black;
			colors.hoverBackground = RGBA{ 0xffffff };

			colors.clickedText = Colors::Black;
			colors.clickedBackground = RGBA{ 0xe5e5e5 };
		}

		return colors;
	}
	TextButton::TextButtonColors TextButton::GetTextButtonAccentedColors() noexcept
	{
		TextButton::TextButtonColors colors;

		if (UIColors::IsDarkMode())
		{
			colors.normalText = Colors::Aliceblue;
			colors.normalBackground = RGBA{ 0x1b1b1b };

			colors.hoverText = RGBA{ 0x1b1b1b };
			colors.hoverBackground = UIColors::GetAccentColor();

			colors.clickedText = RGBA{ 0x1b1b1b };
			colors.clickedBackground = UIColors::GetAccentDark1Color();

		}
		else
		{
			colors.normalText = Colors::Black;
			colors.normalBackground = RGBA{ 0xf3f3f3 };

			colors.hoverText = Colors::Black;
			colors.hoverBackground = UIColors::GetAccentLight2Color();

			colors.clickedText = Colors::Black;
			colors.clickedBackground = UIColors::GetAccentColor();
		}

		return colors;
	}

	TextButton::TextButton(const TextButtonColors& _colors, TextFormat _textFormat) noexcept :
		ButtonBase{ PGUI::Core::WindowClass::Create(L"TextButton_UIControl") },
		colors(_colors), textFormat(_textFormat)
	{
		RegisterMessageHandler(WM_NCCREATE, &TextButton::OnNCCreate);
		RegisterMessageHandler(WM_PAINT, &TextButton::OnPaint);
		RegisterMessageHandler(WM_SIZE, &TextButton::OnSize);

		RegisterMessageHandler(WM_SETTEXT, &TextButton::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &TextButton::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &TextButton::OnGetTextLength);

		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat();
		}

		StateChangedEvent().Subscribe(PGUI::BindMemberFunc(&TextButton::OnStateChanged, this));
		OnStateChanged(GetState());
	}

	TextLayout TextButton::GetTextLayout() const noexcept
	{
		return textLayout;
	}

	void TextButton::SetTextFormat(TextFormat _textFormat) noexcept
	{
		textFormat = _textFormat;

		InitTextLayout();
	}

	void TextButton::InitTextLayout()
	{
		textLayout.Reset();

		auto size = GetClientSize();

		textLayout = TextLayout{ text, textFormat, size };
		textBrush.ReleaseBrush();
	}

	void TextButton::SetText(std::wstring_view newText) noexcept
	{
		text = newText;
		SendMessageW(Hwnd(), WM_SETTEXT, NULL, std::bit_cast<LPARAM>(text.data()));

		InitTextLayout();
		Invalidate();

		textChangedEvent.Emit(text);
	}

	const std::wstring& TextButton::GetText() const noexcept
	{
		return text;
	}

	const TextButton::TextButtonColors& TextButton::GetColors() const noexcept
	{
		return colors;
	}
	TextButton::TextButtonColors& TextButton::GetColors() noexcept
	{
		return colors;
	}

	Core::Event<std::wstring_view>& TextButton::TextChangedEvent() noexcept
	{
		return textChangedEvent;
	}

	void TextButton::OnStateChanged(ButtonState newState) noexcept
	{
		switch (newState)
		{
			using enum PGUI::UI::Controls::ButtonState;

			case Normal:
			{
				textBrush.SetParameters(colors.normalText);
				backgroundBrush.SetParameters(colors.normalBackground);
				break;
			}
			case Hover:
			{
				textBrush.SetParameters(colors.hoverText);
				backgroundBrush.SetParameters(colors.hoverBackground);
				break;
			}
			case Clicked:
			{
				textBrush.SetParameters(colors.clickedText);
				backgroundBrush.SetParameters(colors.clickedBackground);
				break;
			}
		}

		DiscardDeviceResources();
		Invalidate();
	}

	void TextButton::CreateDeviceResources()
	{
		auto renderer = GetRenderingInterface();

		if (!textBrush)
		{
			SetGradientBrushRect(textBrush, textLayout.GetBoundingRect());
			textBrush.CreateBrush(renderer);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			backgroundBrush.CreateBrush(renderer);
		}
	}

	void TextButton::DiscardDeviceResources()
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	Core::HandlerResult TextButton::OnNCCreate(UINT, WPARAM, LPARAM lParam) noexcept
	{
		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		text = createStruct->lpszName;

		return 1;
	}

	Core::HandlerResult TextButton::OnPaint(UINT, WPARAM, LPARAM) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());
		renderer->DrawTextLayout(PointF{ 0, 0 }, textLayout, textBrush->GetBrushPtr());

		HRESULT hr = EndDraw(); HR_L(hr);

		return 0;
	}

	Core::HandlerResult TextButton::OnSize(UINT, WPARAM, LPARAM) noexcept
	{
		InitTextLayout();

		return 0;
	}

	Core::HandlerResult TextButton::OnSetText(UINT, WPARAM, LPARAM lParam) noexcept
	{
		if (auto newText = std::bit_cast<wchar_t*>(lParam);
			text.data() != newText)
		{
			text = newText;
			textChangedEvent.Emit(text);
			InitTextLayout();
		}

		return { 1, Core::HandlerResultFlag::PassToDefWindowProc };
	}
	Core::HandlerResult TextButton::OnGetText(UINT, WPARAM wParam, LPARAM lParam) noexcept
	{
		auto minSize = std::min(text.size() + 1, wParam);

		StringCchCopyNW(std::bit_cast<wchar_t*>(lParam), wParam, text.data(), minSize);

		return minSize;
	}
	Core::HandlerResult TextButton::OnGetTextLength(UINT, WPARAM, LPARAM) const noexcept
	{
		return text.size();
	}
}
