#include "ui/controls/StaticText.hpp"

#include "ui/UIColors.hpp"
#include "ui/Color.hpp"
#include "ui/Colors.hpp"
#include "ui/Gradient.hpp"

#include <algorithm>
#include <strsafe.h>
#include <utility>


namespace PGUI::UI::Controls
{
	static auto GetStaticTextColors() -> std::pair<RGBA, RGBA>
	{
		RGBA textColor;
		RGBA backgroundColor;

		if (UIColors::IsDarkMode())
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
		textFormat(std::move(_textFormat))
	{
		RegisterMessageHandler(WM_NCCREATE, &StaticText::OnNCCreate);
		RegisterMessageHandler(WM_PAINT, &StaticText::OnPaint);
		RegisterMessageHandler(WM_SIZE, &StaticText::OnSize);
		RegisterMessageHandler(WM_SETTEXT, &StaticText::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &StaticText::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &StaticText::OnGetTextLength);

		auto [textColor, backgroundColor] = GetStaticTextColors();
		textBrush.SetParameters(textColor);
		backgroundBrush.SetParameters(backgroundColor);
	}

	auto StaticText::GetTextLayout() const noexcept -> TextLayout
	{
		return textLayout;
	}

	void StaticText::SetTextFormat(TextFormat _textFormat) noexcept
	{
		textFormat = std::move(_textFormat);

		InitTextLayout();
	}

	void StaticText::InitTextLayout()
	{
		textLayout.Reset();

		auto size = GetClientSize();

		textLayout = TextLayout{ text, textFormat, size };
		textBrush.ReleaseBrush();
	}

	void StaticText::SetText(std::wstring_view newText) noexcept
	{
		text = newText;
		SendMessageW(Hwnd(), WM_SETTEXT, NULL, std::bit_cast<LPARAM>(text.data()));
		
		InitTextLayout();
		Invalidate();

		textChangedEvent.Emit(text);
	}

	auto StaticText::GetText() const noexcept -> const std::wstring&
	{
		return text;
	}

	auto StaticText::GetText() noexcept -> std::wstring&
	{
		return text;
	}

	void StaticText::SetTextBrush(const Brush& brush) noexcept
	{
		textBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}

	auto StaticText::GetTextBrush() const noexcept -> const Brush&
	{
		return textBrush;
	}

	void StaticText::SetBackgroundBrush(const Brush& brush) noexcept
	{
		backgroundBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}

	auto StaticText::GetBackgroundBrush() const noexcept -> const Brush&
	{
		return backgroundBrush;
	}

	auto StaticText::TextChangedEvent() noexcept -> Core::Event<std::wstring_view>&
	{
		return textChangedEvent;
	}

	void StaticText::CreateDeviceResources()
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

	void StaticText::DiscardDeviceResources()
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	auto StaticText::OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult
	{
		SetTextFormat(textFormat.AdjustFontSizeToDPI(textFormat.GetFontSize() * dpiScale));

		return Window::OnDPIChange(dpiScale, suggestedRect);
	}

	auto StaticText::OnNCCreate(UINT /*unused*/, WPARAM /*unused*/, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		const auto* createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		text = createStruct->lpszName;
		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat(ScaleByDPI(16.0F));
		}

		return 1;
	}

	auto StaticText::OnPaint(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
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
	auto StaticText::OnSize(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		InitTextLayout();

		return 0;
	}

	auto StaticText::OnSetText(UINT /*unused*/, WPARAM /*unused*/, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		if (const auto* newText = std::bit_cast<wchar_t*>(lParam); 
			text.data() != newText)
		{
			text = newText;
			textChangedEvent.Emit(text);
			InitTextLayout();
		}

		return { 1, Core::HandlerResultFlag::PassToDefWindowProc };
	}
	auto StaticText::OnGetText(UINT /*unused*/, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		auto minSize = std::min(text.size() + 1, wParam);
		
		StringCchCopyNW(std::bit_cast<wchar_t*>(lParam), wParam, text.data(), minSize);

		return minSize;
	}
	auto StaticText::OnGetTextLength(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const noexcept -> Core::HandlerResult
	{
		return text.size();
	}
}
