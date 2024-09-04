#include "ui/controls/TextButton.hpp"

#include "ui/Colors.hpp"
#include "helpers/ScopedTimer.hpp"

#include <algorithm>
#include <strsafe.h>
#include <utility>


namespace PGUI::UI::Controls
{
	auto TextButton::GetTextButtonColors() noexcept -> TextButton::TextButtonColors
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
	auto TextButton::GetTextButtonAccentedColors() noexcept -> TextButton::TextButtonColors
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

	TextButton::TextButton(TextButtonColors  _colors, TextFormat _textFormat) noexcept :
		ButtonBase{ PGUI::Core::WindowClass::Create(L"TextButton_UIControl") },
		colors(std::move(_colors)), textFormat(std::move(_textFormat))
	{
		RegisterMessageHandler(WM_NCCREATE, &TextButton::OnNCCreate);
		RegisterMessageHandler(WM_PAINT, &TextButton::OnPaint);
		RegisterMessageHandler(WM_SIZE, &TextButton::OnSize);

		RegisterMessageHandler(WM_SETTEXT, &TextButton::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &TextButton::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &TextButton::OnGetTextLength);

		StateChangedEvent().Subscribe(PGUI::BindMemberFunc(&TextButton::OnStateChanged, this));
		OnStateChanged(GetState());
	}

	auto TextButton::GetTextLayout() const noexcept -> TextLayout
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

	auto TextButton::GetText() const noexcept -> const std::wstring&
	{
		return text;
	}

	auto TextButton::GetColors() const noexcept -> const TextButton::TextButtonColors&
	{
		return colors;
	}
	auto TextButton::GetColors() noexcept -> TextButton::TextButtonColors&
	{
		return colors;
	}

	auto TextButton::TextChangedEvent() noexcept -> Core::Event<std::wstring_view>&
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
			case Pressed:
			{
				textBrush.SetParameters(colors.clickedText);
				backgroundBrush.SetParameters(colors.clickedBackground);
				break;
			}
			default:
				break;
		}

		DiscardDeviceResources();
		Invalidate();
	}

	void TextButton::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!textBrush)
		{
			SetGradientBrushRect(textBrush, textLayout.GetBoundingRect());
			g.CreateBrush(textBrush);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
		}
	}

	void TextButton::DiscardDeviceResources()
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	auto TextButton::OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult
	{
		SetTextFormat(textFormat.AdjustFontSizeToDPI(textFormat.GetFontSize() * dpiScale));

		return Window::OnDPIChange(dpiScale, suggestedRect);
	}

	auto TextButton::OnNCCreate(UINT, WPARAM, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		text = createStruct->lpszName;
		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat(ScaleByDPI(16.0f));
		}

		return 1;
	}

	auto TextButton::OnPaint(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		BeginDraw();

		auto g = GetGraphics();

		g.Clear(backgroundBrush);

		auto prevTransform = g.GetTransform();
		g.SetTransform(GetDpiScaleTransform(textLayout.GetBoundingRect().Center()));

		g.DrawTextLayout(PointF{ 0, 0 }, textLayout, textBrush);

		g.SetTransform(prevTransform);

		HRESULT hr = EndDraw(); HR_L(hr);

		return 0;
	}

	auto TextButton::OnSize(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		InitTextLayout();

		return 0;
	}

	auto TextButton::OnSetText(UINT, WPARAM, LPARAM lParam) noexcept -> Core::HandlerResult
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
	auto TextButton::OnGetText(UINT, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		auto minSize = std::min(text.size() + 1, wParam);

		StringCchCopyNW(std::bit_cast<wchar_t*>(lParam), wParam, text.data(), minSize);

		return minSize;
	}
	auto TextButton::OnGetTextLength(UINT, WPARAM, LPARAM) const noexcept -> Core::HandlerResult
	{
		return text.size();
	}
}
