#include "ui/controls/StaticText.hpp"

#include "ui/Color.hpp"
#include "ui/Gradient.hpp"

#include <strsafe.h>
#include <algorithm>

#undef min


namespace PGUI::UI::Controls
{
	StaticText::StaticText(TextFormat _textFormat) noexcept :
		UIComponent{ PGUI::Core::WindowClass::Create(L"StaticText_UIComponent") },
		textFormat(_textFormat), textBrush(RGBA{ 0xffffff }), backgroundBrush(RGBA{ 0x1e1e1e })
	{
		RegisterMessageHandler(WM_NCCREATE, &StaticText::OnNCCreate);
		RegisterMessageHandler(WM_PAINT, &StaticText::OnPaint);
		RegisterMessageHandler(WM_SIZE, &StaticText::OnSize);
		RegisterMessageHandler(WM_SETTEXT, &StaticText::OnSetText);
		RegisterMessageHandler(WM_GETTEXT, &StaticText::OnGetText);
		RegisterMessageHandler(WM_GETTEXTLENGTH, &StaticText::OnGetTextLength);

		if (!textFormat)
		{
			textFormat = TextFormat{ L"Segoe UI", 16, GetUserLocaleName() };

			textFormat->SetParagraphAlignment(Font::ParagraphAlignments::Center);
			textFormat->SetTextAlignment(Font::TextAlignments::Center);
		}
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

	void StaticText::CreateDeviceResources()
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

	void StaticText::DiscardDeviceResources()
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
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

	void StaticText::SetTextBrush(const Brush& brush) noexcept
	{
		textBrush.SetParameters(brush.GetParameters());
	}

	const Brush& StaticText::GetTextBrush() const noexcept
	{
		return textBrush;
	}

	void StaticText::SetBackgroundBrush(const Brush& brush) noexcept
	{
		backgroundBrush.SetParameters(brush.GetParameters());
	}

	const Brush& StaticText::GetBackgroundBrush() const noexcept
	{
		return backgroundBrush;
	}

	Core::Event<std::wstring_view>& StaticText::TextChangedEvent() noexcept
	{
		return textChangedEvent;
	}

	Core::HandlerResult StaticText::OnNCCreate(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, LPARAM lParam)
	{
		auto createStruct = std::bit_cast<LPCREATESTRUCTW>(lParam);

		text = createStruct->lpszName;

		return 1;
	}

	Core::HandlerResult StaticText::OnPaint(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		renderer->Clear(RGBA{ 0 });
		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());
		renderer->DrawTextLayout(PointF{ 0, 0 }, textLayout, textBrush->GetBrushPtr());

		HRESULT hr = EndDraw(); HR_L(hr);

		return 0;
	}
	Core::HandlerResult StaticText::OnSize(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
	{
		InitTextLayout();

		return 0;
	}

	Core::HandlerResult StaticText::OnSetText(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
	{
		if (auto newText = std::bit_cast<wchar_t*>(lParam); 
			text.data() != newText)
		{
			text = newText;
			textChangedEvent.Emit(text);
			InitTextLayout();
		}

		return { 1, Core::HandlerResultFlags::PassToDefWindowProc };
	}
	Core::HandlerResult StaticText::OnGetText(
		[[maybe_unused]] UINT msg, WPARAM wParam, LPARAM lParam)
	{
		auto minSize = std::min(text.size() + 1, wParam);
		
		StringCchCopyNW(std::bit_cast<wchar_t*>(lParam), wParam, text.data(), minSize);

		return minSize;
	}
	Core::HandlerResult StaticText::OnGetTextLength(
		[[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam) const
	{
		return text.size();
	}
}
