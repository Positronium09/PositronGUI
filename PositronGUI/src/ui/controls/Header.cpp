#include "ui/controls/Header.hpp"

#include "ui/Colors.hpp"
#include "ui/UIColors.hpp"

#include <algorithm>
#include <numeric>
#include <ranges>
#include <utility>


namespace PGUI::UI::Controls
{
	void HeaderItem::Invalidate() const noexcept
	{
		header->Invalidate();
	}

	#pragma region HeaderTextItem

	auto HeaderTextItem::GetHeaderTextItemColors() noexcept -> HeaderTextItem::HeaderTextItemColors
	{
		HeaderTextItemColors colors;

		if (UIColors::IsDarkMode())
		{
			colors.normalText = Colors::Aliceblue;
			colors.normalBackground = RGBA{ 0x1b1b1b };

			colors.hoverText = Colors::Aliceblue;
			colors.hoverBackground = RGBA{ 0x202020 };

			colors.pressedText = Colors::Aliceblue;
			colors.pressedBackground = RGBA{ 0x191919 };

		}
		else
		{
			colors.normalText = Colors::Black;
			colors.normalBackground = RGBA{ 0xf3f3f3 };

			colors.hoverText = Colors::Black;
			colors.hoverBackground = RGBA{ 0xffffff };

			colors.pressedText = Colors::Black;
			colors.pressedBackground = RGBA{ 0xe5e5e5 };
		}

		return colors;
	}
	auto HeaderTextItem::GetHeaderTextItemAccentedColors() noexcept -> HeaderTextItem::HeaderTextItemColors
	{
		HeaderTextItemColors colors;

		if (UIColors::IsDarkMode())
		{
			colors.normalText = Colors::Aliceblue;
			colors.normalBackground = RGBA{ 0x1b1b1b };

			colors.hoverText = RGBA{ 0x1b1b1b };
			colors.hoverBackground = UIColors::GetAccentColor();

			colors.pressedText = RGBA{ 0x1b1b1b };
			colors.pressedBackground = UIColors::GetAccentDark1Color();

		}
		else
		{
			colors.normalText = Colors::Black;
			colors.normalBackground = RGBA{ 0xf3f3f3 };

			colors.hoverText = Colors::Black;
			colors.hoverBackground = UIColors::GetAccentLight2Color();

			colors.pressedText = Colors::Black;
			colors.pressedBackground = UIColors::GetAccentColor();
		}

		return colors;
	}

	HeaderTextItem::HeaderTextItem(std::wstring_view text,
		long width, 
		HeaderTextItemColors  colors,
		TextFormat tf) noexcept :
		HeaderItem{ width },
		text{ text },
		colors{std::move( colors )}, textFormat{std::move( tf )}
	{
		WidthChangedEvent().Subscribe([this]()
		{
			InitTextLayout();
		});

		StateChangedEvent().Subscribe(BindMemberFunc(&HeaderTextItem::OnStateChanged, this));
	}

	void HeaderTextItem::SetTextFormat(TextFormat _textFormat) noexcept
	{
		textFormat = std::move(_textFormat);
		InitTextLayout();
	}

	void HeaderTextItem::InitTextLayout()
	{
		auto height = GetHeaderWindow()->GetClientSize().cy;
		textLayout = TextLayout{ text, textFormat, 
			SizeF {
				static_cast<float>(GetWidth()),
				static_cast<float>(height) 
			}
		};
		textBrush.ReleaseBrush();
	}

	void HeaderTextItem::OnStateChanged() noexcept
	{
		using enum HeaderItemState;
		switch (GetState())
		{
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
				textBrush.SetParameters(colors.pressedText);
				backgroundBrush.SetParameters(colors.pressedBackground);
				break;
			}
		}
		DiscardDeviceResources(Graphics::Graphics{ nullptr });
		Invalidate();
	}

	void HeaderTextItem::Create()
	{
		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat(GetHeaderWindow()->ScaleByDPI(16.0F));
		}
		InitTextLayout();
		OnStateChanged();
	}
	void HeaderTextItem::Render(Graphics::Graphics g, RectF renderRect)
	{
		auto prevTransform = g.GetTransform();
		g.SetTransform(GetHeaderWindow()->GetDpiScaleTransform(renderRect.Center()));
		
		g.FillRect(renderRect, backgroundBrush);
		g.DrawTextLayout(renderRect.TopLeft(), textLayout, textBrush);

		g.SetTransform(prevTransform);
	}

	void HeaderTextItem::CreateDeviceResources(Graphics::Graphics g)
	{
		if (!textBrush)
		{
			SetGradientBrushRect(textBrush, textLayout.GetBoundingRect());
			g.CreateBrush(textBrush);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetHeaderWindow()->GetClientRect());
			g.CreateBrush(backgroundBrush);
		}
	}

	void HeaderTextItem::DiscardDeviceResources(Graphics::Graphics /*g*/)
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	void HeaderTextItem::OnDPIChanged(float dpiScale)
	{
		SetWidth(ScaleForDPI(GetWidth(), dpiScale));
		SetTextFormat(textFormat.AdjustFontSizeToDPI(textFormat.GetFontSize() * dpiScale));
	}

	void HeaderTextItem::OnHeaderSizeChanged()
	{
		InitTextLayout();
	}

	#pragma endregion

	#pragma region Header

	Header::Header() : 
		Control{ Core::WindowClass::Create(L"Header_UIControl") }
	{
		RegisterMessageHandler(WM_PAINT, &Header::OnPaint);
		RegisterMessageHandler(WM_MOUSEMOVE, &Header::OnMouseMove);
		RegisterMessageHandler(WM_LBUTTONDOWN, &Header::OnMouseLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &Header::OnMouseLButtonUp);
		RegisterMessageHandler(WM_MOUSELEAVE, &Header::OnMouseLeave);
		RegisterMessageHandler(WM_SETCURSOR, &Header::OnSetCursor);
		RegisterMessageHandler(WM_SIZE, &Header::OnSize);

		if (UIColors::IsDarkMode())
		{
			separatorBrush.SetParameters(RGBA{ 0x272727 });
			backgroundBrush.SetParameters(RGBA{ 0x181818 });
		}
		else
		{
			separatorBrush.SetParameters(RGBA{ 0x070707 });
			backgroundBrush.SetParameters(Colors::White);
		}
	}

	void Header::SetSeparatorBrush(const Brush& _separatorBrush) noexcept
	{
		separatorBrush.SetParameters(_separatorBrush.GetParameters());
		Invalidate();
	}
	void Header::SetBackgroundBrush(const Brush& _backgroundBrush) noexcept
	{
		backgroundBrush.SetParameters(_backgroundBrush.GetParameters());
		Invalidate();
	}

	void Header::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!separatorBrush)
		{
			g.CreateBrush(separatorBrush);
		}
		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
		}

		std::ranges::for_each(headerItems, [g](const auto& headerItem)
		{
			headerItem->CreateDeviceResources(g);
		});
	}
	void Header::DiscardDeviceResources()
	{
		auto g = GetGraphics();

		separatorBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();

		std::ranges::for_each(headerItems, [g](const auto& headerItem)
		{
			headerItem->DiscardDeviceResources(g);
		});
	}

	auto Header::GetHoveredHeaderItemIndex(long xPos) const noexcept -> std::optional<std::size_t>
	{
		long totalWidth = 0;

		for (const auto& [index, headerItem] : headerItems | std::views::enumerate)
		{
			auto width = headerItem->GetWidth();

			if (totalWidth <= xPos && 
				xPos <= totalWidth + width + sizingMargin)
			{
				return index;
			}

			totalWidth += width;
		}

		return std::nullopt;
	}
	auto Header::IsMouseOnSeparator(long xPos) const noexcept -> bool
	{
		long totalWidth = 0;

		for (const auto& headerItem : headerItems)
		{
			auto width = headerItem->GetWidth();

			if (totalWidth + width - sizingMargin <= xPos &&
				xPos <= totalWidth + width + sizingMargin)
			{
				return true;
			}

			totalWidth += width;
		}

		return false;
	}

	auto Header::CalculateHeaderItemWidthUpToIndex(std::size_t index) const noexcept -> long
	{
		return std::accumulate(
			headerItems.cbegin(), std::next(headerItems.cbegin(), static_cast<std::ptrdiff_t>(index)), 0,
			[](long x, const auto& header) { return x + header->GetWidth(); }
		);
	}
	auto Header::GetTotalHeaderWidth() const noexcept -> long
	{
		return std::accumulate(
			headerItems.cbegin(), headerItems.cend(), 0,
			[](long x, const auto& header) { return x + header->GetWidth(); }
		);
	}

	auto Header::OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult
	{
		std::ranges::for_each(headerItems, [dpiScale](const auto& item)
		{
			item->OnDPIChanged(dpiScale);
		});

		return Window::OnDPIChange(dpiScale, suggestedRect);
	}

	auto Header::OnPaint(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) -> Core::HandlerResult
	{
		BeginDraw();

		long totalWidth = 0;
		long width = GetClientSize().cx;
		long height = GetClientSize().cy;

		auto g = GetGraphics();

		g.Clear(backgroundBrush);

		for (const auto& headerItem : headerItems)
		{
			if (totalWidth > width)
			{
				break;
			}

			auto headerRect = RectF{
				static_cast<float>(totalWidth),
				0,
				static_cast<float>(totalWidth + headerItem->GetWidth()),
				static_cast<float>(height)
			};

			g.PushAxisAlignedClip(headerRect, g.GetAntialiasMode());

			headerItem->Render(g, headerRect);

			g.PopAxisAlignedClip();

			totalWidth += headerItem->GetWidth();
			g.DrawLine(
				PointF{ static_cast<float>(totalWidth - 1), 0 }, 
				PointF{ static_cast<float>(totalWidth - 1), static_cast<float>(height) },
				separatorBrush, ScaleByDPI(1.3F));
		}

		EndDraw();

		return 0;
	}
	
	auto Header::OnMouseMove(UINT /*unused*/, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult
	{
		TRACKMOUSEEVENT tme{ };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = Hwnd();
		TrackMouseEvent(&tme);

		PointL mousePos = MAKEPOINTS(lParam);

		if (dragging && wParam & MK_LBUTTON && hoveringIndex.has_value())
		{
			auto* ptr = GetItem(*hoveringIndex);

			if (long newWidth = mousePos.x - 
				CalculateHeaderItemWidthUpToIndex(*hoveringIndex);
				newWidth >= ptr->GetMinWidth())
			{
				ptr->SetWidth(newWidth);
				ptr->WidthChangedEvent().Emit();
				Invalidate();
			}

			return 0;
		}

		const auto prevHoveredHeaderIndex = hoveringIndex;
		hoveringIndex = GetHoveredHeaderItemIndex(mousePos.x);
		mouseOnDivider = IsMouseOnSeparator(mousePos.x);

		if (mousePos.x > GetTotalHeaderWidth() + sizingMargin)
		{
			if (prevHoveredHeaderIndex.has_value())
			{
				headerItems.at(*prevHoveredHeaderIndex)->SetState(HeaderItemState::Normal);
				Invalidate();
			}
			return 0;
		}

		if (prevHoveredHeaderIndex.has_value() &&
			prevHoveredHeaderIndex < headerItems.size() && 
			*prevHoveredHeaderIndex != *hoveringIndex)
		{
			headerItems.at(*prevHoveredHeaderIndex)->SetState(HeaderItemState::Normal);
		}
		if (hoveringIndex.has_value() && 
			hoveringIndex < headerItems.size() &&
			headerItems.at(*hoveringIndex)->GetState() != HeaderItemState::Pressed)
		{
			headerItems.at(*hoveringIndex)->SetState(HeaderItemState::Hover);
		}

		if (prevHoveredHeaderIndex != hoveringIndex)
		{
			Invalidate();
		}

		return 0;
	}
	auto Header::OnMouseLButtonDown(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) -> Core::HandlerResult
	{
		if (dragging)
		{
			return 0;
		}

		if (mouseOnDivider)
		{
			dragging = true;
			SetCapture(Hwnd());
		}
		else if (hoveringIndex.has_value())
		{
			headerItems.at(*hoveringIndex)->SetState(HeaderItemState::Pressed);
		}

		return 0;
	}
	auto Header::OnMouseLButtonUp(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) -> Core::HandlerResult
	{
		if (dragging)
		{
			dragging = false;
			ReleaseCapture();
		}
		else if (hoveringIndex.has_value())
		{
			headerItems.at(*hoveringIndex)->SetState(HeaderItemState::Hover);
			headerItemClickedEvent.Emit(*hoveringIndex);
		}

		return 0;
	}
	auto Header::OnMouseLeave(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) -> Core::HandlerResult
	{
		if (hoveringIndex.has_value())
		{
			headerItems.at(*hoveringIndex)->SetState(HeaderItemState::Normal);
			Invalidate();
		}
		hoveringIndex = std::nullopt;

		return 0;
	}

	auto Header::OnSetCursor(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const -> Core::HandlerResult
	{
		LPCWSTR cursorName = nullptr;
		if (mouseOnDivider)
		{
			cursorName = IDC_SIZEWE;
		}
		else
		{
			cursorName = IDC_ARROW;
		}

		auto* hCursor = static_cast<HCURSOR>(LoadImageW(nullptr, cursorName, IMAGE_CURSOR, NULL, NULL, LR_SHARED | LR_DEFAULTSIZE));
		if (!hCursor)
		{
			HR_L(HresultFromWin32());
			return 0;
		}

		SetCursor(hCursor);

		return 1;
	}

	auto Header::OnSize(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const -> Core::HandlerResult
	{
		std::ranges::for_each(headerItems, [](const auto& item)
		{
			item->OnHeaderSizeChanged();
		});
		
		return 0;
	}

	#pragma endregion
}
