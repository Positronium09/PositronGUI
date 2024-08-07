#include "ui/controls/Header.hpp"

#include "ui/Colors.hpp"
#include "ui/UIColors.hpp"

#include <numeric>
#include <ranges>
#include <algorithm>


namespace PGUI::UI::Controls
{
	void HeaderItem::Invalidate() const noexcept
	{
		header->Invalidate();
	}

	#pragma region HeaderTextItem

	HeaderTextItem::HeaderTextItemColors 
		HeaderTextItem::GetHeaderTextItemColors() noexcept
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
	HeaderTextItem::HeaderTextItemColors 
		HeaderTextItem::GetHeaderTextItemAccentedColors() noexcept
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
		const HeaderTextItemColors& colors,
		TextFormat tf) noexcept :
		HeaderItem{ width },
		text{ text },
		colors{ colors }, textFormat{ tf }
	{
		GetWidthChangedEvent().Subscribe([this]()
		{
			InitTextLayout();
		});

		GetStateChangedEvent().Subscribe(BindMemberFunc(&HeaderTextItem::OnStateChanged, this));
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
		switch (GetState())
		{
			using enum HeaderItemState;

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

	void HeaderTextItem::DiscardDeviceResources(Graphics::Graphics)
	{
		textBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
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
			seperatorBrush.SetParameters(RGBA{ 0x272727 });
			backgroundBrush.SetParameters(RGBA{ 0x181818 });
		}
		else
		{
			seperatorBrush.SetParameters(RGBA{ 0x070707 });
			backgroundBrush.SetParameters(Colors::White);
		}
	}

	void Header::SetSeperatorBrush(const Brush& _seperatorBrush) noexcept
	{
		seperatorBrush.SetParameters(_seperatorBrush.GetParameters());
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

		if (!seperatorBrush)
		{
			g.CreateBrush(seperatorBrush);
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

		seperatorBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();

		std::ranges::for_each(headerItems, [g](const auto& headerItem)
		{
			headerItem->DiscardDeviceResources(g);
		});
	}

	std::optional<std::size_t> Header::GetHoveredHeaderItemIndex(long xPos) const noexcept
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
	bool Header::IsMouseOnSeperator(long xPos) const noexcept
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

	long Header::CalculateHeaderItemWidthUpToIndex(std::size_t index) const noexcept
	{
		return std::accumulate(
			headerItems.cbegin(), std::next(headerItems.cbegin(), index), 0,
			[](long x, const auto& header) { return x + header->GetWidth(); }
		);
	}
	long Header::GetTotalHeaderWidth() const noexcept
	{
		return std::accumulate(
			headerItems.cbegin(), headerItems.cend(), 0,
			[](long x, const auto& header) { return x + header->GetWidth(); }
		);
	}

	Core::HandlerResult Header::OnPaint(UINT, WPARAM, LPARAM)
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
				seperatorBrush, ScaleByDpi(1.3f));
		}

		EndDraw();

		return 0;
	}
	
	Core::HandlerResult Header::OnMouseMove(UINT, WPARAM wParam, LPARAM lParam)
	{
		TRACKMOUSEEVENT tme{ };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = Hwnd();
		TrackMouseEvent(&tme);

		PointL mousePos = MAKEPOINTS(lParam);

		if (dragging && wParam & MK_LBUTTON && hoveringIndex.has_value())
		{
			auto ptr = GetItem(*hoveringIndex);

			if (long newWidth = mousePos.x - 
				CalculateHeaderItemWidthUpToIndex(*hoveringIndex);
				newWidth >= ptr->GetMinWidth())
			{
				ptr->SetWidth(newWidth);
				ptr->GetWidthChangedEvent().Emit();
				Invalidate();
			}

			return 0;
		}

		const auto prevHoveredHeaderIndex = hoveringIndex;
		hoveringIndex = GetHoveredHeaderItemIndex(mousePos.x);
		mouseOnDivider = IsMouseOnSeperator(mousePos.x);

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
	Core::HandlerResult Header::OnMouseLButtonDown(UINT, WPARAM, LPARAM)
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
	Core::HandlerResult Header::OnMouseLButtonUp(UINT, WPARAM, LPARAM)
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
	Core::HandlerResult Header::OnMouseLeave(UINT, WPARAM, LPARAM)
	{
		if (hoveringIndex.has_value())
		{
			headerItems.at(*hoveringIndex)->SetState(HeaderItemState::Normal);
			Invalidate();
		}
		hoveringIndex = std::nullopt;

		return 0;
	}

	Core::HandlerResult Header::OnSetCursor(UINT, WPARAM, LPARAM) const
	{
		LPCWSTR cursorName;
		if (mouseOnDivider)
		{
			cursorName = IDC_SIZEWE;
		}
		else
		{
			cursorName = IDC_ARROW;
		}

		auto hCursor = static_cast<HCURSOR>(LoadImageW(nullptr, cursorName, IMAGE_CURSOR, NULL, NULL, LR_SHARED | LR_DEFAULTSIZE));
		if (!hCursor)
		{
			HR_L(HresultFromWin32());
			return 0;
		}

		SetCursor(hCursor);

		return 1;
	}

	Core::HandlerResult Header::OnSize(UINT, WPARAM, LPARAM) const
	{
		std::ranges::for_each(headerItems, [](const auto& item)
		{
			item->OnHeaderSizeChanged();
		});
		
		return 0;
	}

	#pragma endregion
}
