#include "ui/controls/StaticText.hpp"

#include "ui/UIColors.hpp"
#include "ui/Color.hpp"
#include "ui/Colors.hpp"
#include "ui/Gradient.hpp"

#include <algorithm>
#include <strsafe.h>

#undef min


namespace PGUI::UI::Controls
{
	static std::pair<RGBA, RGBA> GetStaticTextColors()
	{
		RGBA textColor;
		RGBA backgroundColor;

		if (const auto uiColors = UIColors::GetInstance(); 
			uiColors->IsDarkMode())
		{
			textColor = Colors::Aliceblue;
			backgroundColor = RGBA{ 0x1b1b1b };
		}
		else
		{
			textColor = Colors::Black;
			backgroundColor = RGBA{ 0xf3f3f3 };
		}

		return { textColor, backgroundColor };
	}

	StaticText::StaticText(TextFormat _textFormat) noexcept :
		UIComponent{ PGUI::Core::WindowClass::Create(L"StaticText_UIComponent") },
		textFormat(_textFormat)
	{
		RegisterMessageHandler(WM_NCCREATE, &StaticText::OnNCCreate);
		RegisterMessageHandler(WM_PAINT, &StaticText::OnPaint);
		RegisterMessageHandler(WM_SIZE, &StaticText::OnSize);
		RegisterMessageHandler(WM_SETTEXT, &StaticText::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &StaticText::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &StaticText::OnGetTextLength);

		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat();
		}

		auto [textColor, backgroundColor] = GetStaticTextColors();
		textBrush.SetParameters(textColor);
		backgroundBrush.SetParameters(backgroundColor);
	}

	TextLayout StaticText::GetTextLayout() const noexcept
	{
		return textLayout;
	}

	void StaticText::SetTextFormat(TextFormat _textFormat) noexcept
	{
		textFormat = _textFormat;

		InitTextLayout();
	}

	void StaticText::InitTextLayout()
	{
		textLayout.Reset();

		auto size = GetClientSize();

		textLayout = TextLayout{ text, textFormat, size };
	}

	void StaticText::SetText(std::wstring_view newText) noexcept
	{
		text = newText;
		SendMessageW(Hwnd(), WM_SETTEXT, NULL, std::bit_cast<LPARAM>(text.data()));
		
		InitTextLayout();
		Invalidate();

		textChangedEvent.Emit(text);
	}

	const std::wstring& StaticText::GetText() const noexcept
	{
		return text;
	}

	std::wstring& StaticText::GetText() noexcept
	{
		return text;
	}

	void StaticText::SetTextBrush(const Brush& brush) noexcept
	{
		textBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}

	const Brush& StaticText::GetTextBrush() const noexcept
	{
		return textBrush;
	}

	void StaticText::SetBackgroundBrush(const Brush& brush) noexcept
	{
		backgroundBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}

	const Brush& StaticText::GetBackgroundBrush() const noexcept
	{
		return backgroundBrush;
	}

	Core::Event<std::wstring_view>& StaticText::TextChangedEvent() noexcept
	{
		return textChangedEvent;
	}

	void StaticText::CreateDeviceResources()
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

	void StaticText::DiscardDeviceResources()
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	Core::HandlerResult StaticText::OnNCCreate(UINT, WPARAM, LPARAM lParam) noexcept
	{
		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		text = createStruct->lpszName;

		return 1;
	}

	Core::HandlerResult StaticText::OnPaint(UINT, WPARAM, LPARAM) noexcept
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());
		renderer->DrawTextLayout(PointF{ 0, 0 }, textLayout, textBrush->GetBrushPtr());

		HRESULT hr = EndDraw(); HR_L(hr);

		return 0;
	}
	Core::HandlerResult StaticText::OnSize(UINT, WPARAM, LPARAM) noexcept
	{
		InitTextLayout();

		return 0;
	}

	Core::HandlerResult StaticText::OnSetText(UINT, WPARAM, LPARAM lParam) noexcept
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
	Core::HandlerResult StaticText::OnGetText(UINT, WPARAM wParam, LPARAM lParam) noexcept
	{
		auto minSize = std::min(text.size() + 1, wParam);
		
		StringCchCopyNW(std::bit_cast<wchar_t*>(lParam), wParam, text.data(), minSize);

		return minSize;
	}
	Core::HandlerResult StaticText::OnGetTextLength(UINT, WPARAM, LPARAM) const noexcept
	{
		return text.size();
	}
}
