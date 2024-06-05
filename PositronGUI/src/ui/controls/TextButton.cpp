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

		if (const auto uiColors = UIColors::GetInstance();
			uiColors->IsDarkMode())
		{
			colors.normalTextColor = Colors::Aliceblue;
			colors.normalBackgroundColor = RGBA{ 0x1b1b1b };

			colors.hoverTextColor = Colors::Aliceblue;
			colors.hoverBackgroundColor = RGBA{ 0x202020 };

			colors.clickedTextColor = Colors::Aliceblue;
			colors.clickedBackgroundColor = RGBA{ 0x191919 };

		}
		else
		{
			colors.normalTextColor = Colors::Black;
			colors.normalBackgroundColor = RGBA{ 0xf3f3f3 };

			colors.hoverTextColor = Colors::Black;
			colors.hoverBackgroundColor = RGBA{ 0xffffff };

			colors.clickedTextColor = Colors::Black;
			colors.clickedBackgroundColor = RGBA{ 0xe5e5e5 };
		}

		return colors;
	}
	TextButton::TextButtonColors TextButton::GetTextButtonAccentedColors() noexcept
	{
		TextButton::TextButtonColors colors;

		if (const auto uiColors = UIColors::GetInstance();
			uiColors->IsDarkMode())
		{
			colors.normalTextColor = Colors::Aliceblue;
			colors.normalBackgroundColor = RGBA{ 0x1b1b1b };

			colors.hoverTextColor = RGBA{ 0x1b1b1b };
			colors.hoverBackgroundColor = uiColors->GetAccentColor();

			colors.clickedTextColor = RGBA{ 0x1b1b1b };
			colors.clickedBackgroundColor = uiColors->GetAccentDark1Color();

		}
		else
		{
			colors.normalTextColor = Colors::Black;
			colors.normalBackgroundColor = RGBA{ 0xf3f3f3 };

			colors.hoverTextColor = Colors::Black;
			colors.hoverBackgroundColor = uiColors->GetAccentLight2Color();

			colors.clickedTextColor = Colors::Black;
			colors.clickedBackgroundColor = uiColors->GetAccentColor();
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
			textFormat = TextFormat{ L"Segoe UI", 16, GetUserLocaleName() };

			textFormat.SetParagraphAlignment(Font::ParagraphAlignments::Center);
			textFormat.SetTextAlignment(Font::TextAlignments::Center);
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
				textBrush.SetParameters(colors.normalTextColor);
				backgroundBrush.SetParameters(colors.normalBackgroundColor);
				break;
			}
			case Hover:
			{
				textBrush.SetParameters(colors.hoverTextColor);
				backgroundBrush.SetParameters(colors.hoverBackgroundColor);
				break;
			}
			case Clicked:
			{
				textBrush.SetParameters(colors.clickedTextColor);
				backgroundBrush.SetParameters(colors.clickedBackgroundColor);
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
