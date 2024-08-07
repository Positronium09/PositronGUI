#include "ui/controls/ListView.hpp"

#include "ui/UIColors.hpp"
#include "ui/Colors.hpp"

#include <windowsx.h>
#include <span>
#include <ranges>
#include <numeric>
#include <algorithm>


namespace PGUI::UI::Controls
{
	void ListViewItem::Invalidate() const noexcept
	{
		listView->Invalidate();
	}

	#pragma region ListViewTextItem

	ListViewTextItem::ListViewTextItem(std::wstring_view text, long height, TextFormat _textFormat) noexcept :
		ListViewItem{ height },
		text{ text }, textFormat{ _textFormat }
	{
		if (!textFormat)
		{
			textFormat = TextFormat::GetDefTextFormat();
		}
		selectedIndicatorBrush.SetParameters(UIColors::GetAccentColor());

		if (UIColors::IsDarkMode())
		{
			textBrush = Colors::Aliceblue;
			normalBrush = RGBA{ 0x1b1b1b };
			hoverBrush = RGBA{ 0x202020 };
			pressedBrush = RGBA{ 0x191919 };
		}
		else
		{
			textBrush = Colors::Black;
			normalBrush = RGBA{ 0xf3f3f3 };
			hoverBrush = RGBA{ 0xffffff };
			pressedBrush = RGBA{ 0xe5e5e5 };
		}
	}

	void ListViewTextItem::SetTextFormat(TextFormat _textFormat) noexcept
	{
		textFormat = _textFormat;
		InitTextLayout();
	}

	void ListViewTextItem::InitTextLayout()
	{
		textLayout.Reset();

		auto width = GetListViewWindow()->GetClientSize().cx;

		textLayout = TextLayout{ text, textFormat,
			SizeF{
				static_cast<float>(width),
				static_cast<float>(GetHeight())
		} };
		textBrush.ReleaseBrush();
	}

	void ListViewTextItem::SetColors(const ListViewTextItemColors& colors) noexcept
	{
		normalBrush.SetParameters(colors.normal);
		hoverBrush.SetParameters(colors.hover);
		pressedBrush.SetParameters(colors.pressed);
		selectedIndicatorBrush.SetParameters(colors.selectedIndicator);
		textBrush.SetParameters(colors.text);

		DiscardDeviceResources(Graphics::Graphics{ nullptr });
		Invalidate();
	}

	ListViewTextItem::ListViewTextItemColors ListViewTextItem::GetColors() const noexcept
	{
		ListViewTextItemColors colors;
		colors.normal = normalBrush.GetParameters();
		colors.hover = hoverBrush.GetParameters();
		colors.pressed = pressedBrush.GetParameters();
		colors.selectedIndicator = selectedIndicatorBrush.GetParameters();
		colors.text = textBrush.GetParameters();

		return colors;
	}

	void ListViewTextItem::Create()
	{
		InitTextLayout();
	}
	void ListViewTextItem::Render(Graphics::Graphics g, RectF renderRect)
	{
		ListViewItemState currentState = GetState();

		switch (currentState)
		{
			using enum ListViewItemState;

			case Normal:
			case Selected:
			{
				g.FillRect(renderRect, normalBrush);
				break;
			}
			case Hover:
			case Selected | Hover:
			{
				g.FillRect(renderRect, hoverBrush);
				break;
			}
			case Pressed:
			case Selected | Pressed:
			{
				g.FillRect(renderRect, pressedBrush);
				break;
			}
			default:
				break;
		}

		auto prevTransform = g.GetTransform();
		g.SetTransform(GetListViewWindow()->GetDpiScaleTransform(renderRect.Center()));

		if (IsFlagSet(currentState, ListViewItemState::Selected))
		{
			auto selectedIndicatorRect = renderRect;
			selectedIndicatorRect.top += 10;
			selectedIndicatorRect.bottom -= 10;
			selectedIndicatorRect.left -= 5;
			selectedIndicatorRect.right = selectedIndicatorRect.left + 10;

			g.FillRoundedRect(RoundedRect{ selectedIndicatorRect, 3.5f, 5 }, selectedIndicatorBrush);
		}

		g.DrawTextLayout(renderRect.TopLeft(), textLayout, textBrush);

		g.SetTransform(prevTransform);
	}
	void ListViewTextItem::CreateDeviceResources(Graphics::Graphics g)
	{
		if (!normalBrush)
		{
			SetGradientBrushRect(normalBrush, GetListViewWindow()->GetClientRect());
			g.CreateBrush(normalBrush);
		}

		if (!hoverBrush)
		{
			SetGradientBrushRect(hoverBrush, GetListViewWindow()->GetClientRect());
			g.CreateBrush(hoverBrush);
		}

		if (!pressedBrush)
		{
			SetGradientBrushRect(pressedBrush, GetListViewWindow()->GetClientRect());
			g.CreateBrush(pressedBrush);
		}

		if (!textBrush)
		{
			SetGradientBrushRect(normalBrush, textLayout.GetBoundingRect());
			g.CreateBrush(textBrush);
		}
		if (!selectedIndicatorBrush)
		{
			g.CreateBrush(selectedIndicatorBrush);
		}
	}
	void ListViewTextItem::DiscardDeviceResources(Graphics::Graphics g)
	{
		normalBrush.ReleaseBrush();
		hoverBrush.ReleaseBrush();
		pressedBrush.ReleaseBrush();
		textBrush.ReleaseBrush();
		selectedIndicatorBrush.ReleaseBrush();
	}

	void ListViewTextItem::OnListViewSizeChanged()
	{
		InitTextLayout();
	}

	#pragma endregion

	#pragma region ListView

	ListView::ListView() noexcept :
		Control{ Core::WindowClass::Create(L"ListView_UIControl") }
	{
		RegisterMessageHandler(WM_CREATE, &ListView::OnCreate);
		RegisterMessageHandler(WM_PAINT, &ListView::OnPaint);
		RegisterMessageHandler(WM_MOUSEMOVE, &ListView::OnMouseMove);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ListView::OnMouseLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &ListView::OnMouseLButtonUp);
		RegisterMessageHandler(WM_MOUSELEAVE, &ListView::OnMouseLeave);
		RegisterMessageHandler(WM_MOUSEWHEEL, &ListView::OnMouseWheel);
		RegisterMessageHandler(WM_SIZE, &ListView::OnSize);

		itemsChangedEvent.Subscribe(BindMemberFunc(&ListView::UpdateScrollBar, this));

		if (UIColors::IsDarkMode())
		{
			backgroundBrush.SetParameters(RGBA{ 0x181818 });
		}
		else
		{
			backgroundBrush.SetParameters(Colors::White);
		}

		for (const auto& item : listViewItems)
		{
			item->Create();
		}
	}

	void ListView::RemoveItem(std::size_t index) noexcept
	{
		if (index >= listViewItems.size())
		{
			return;
		}
		
		bool wasSelected = listViewItems.at(index)->IsSelected();

		listViewItems.erase(std::ranges::next(listViewItems.begin(), index));
		UpdateScrollBar();
		
		if (wasSelected)
		{
			selectionChangedEvent.Emit();
		}
	}
	
	void ListView::Clear() noexcept
	{
		listViewItems.clear();
		scrollBar->Show(false);
		selectionChangedEvent.Emit();
	}
	void ListView::ClearSelected() noexcept
	{
		auto ret = std::ranges::remove_if(listViewItems, [](const auto& item) -> bool
		{
			return item->IsSelected();
		});
		listViewItems.erase(ret.begin(), ret.end());
		UpdateScrollBar();
		selectionChangedEvent.Emit();
	}
	void ListView::ClearUnselected() noexcept
	{
		auto ret = std::ranges::remove_if(listViewItems, [](const auto& item) -> bool
		{
			return !item->IsSelected();
		});
		listViewItems.erase(ret.begin(), ret.end());
		UpdateScrollBar();
		selectionChangedEvent.Emit();
	}

	void ListView::SelectAll() noexcept
	{
		if (selectionMode == SelectionMode::Single)
		{
			Select(0);
			return;
		}

		for (auto i : std::views::iota(0ULL, listViewItems.size()))
		{
			AddSelected(i);
		}
		selectionChangedEvent.Emit();
	}
	void ListView::DeselectAll() noexcept
	{
		for (auto i : std::views::iota(0ULL, listViewItems.size()))
		{
			RemoveSelected(i);
		}
		selectionChangedEvent.Emit();
	}

	std::vector<ListViewItem*> ListView::GetSelectedItems() const noexcept
	{
		std::vector<ListViewItem*> selectedItems;
		std::ranges::for_each(listViewItems, [&selectedItems](const auto& item)
		{
			if (item->IsSelected())
			{
				selectedItems.push_back(item.get());
			}
		});

		return selectedItems;
	}

	void ListView::SetBackgroundBrush(Brush& brush) noexcept
	{
		backgroundBrush.SetParameters(brush.GetParameters());
	}
	void ListView::SetSelectionMode(SelectionMode _selectionMode) noexcept
	{
		if (selectionMode == _selectionMode)
		{
			return;
		}

		if (auto selectedItemIndex = GetSelectedItemIndex();
			_selectionMode == SelectionMode::Single && selectedItemIndex.has_value())
		{
			auto selectedItemIndexes = GetSelectedItemIndexes();
			std::ranges::for_each(
				std::ranges::next(selectedItemIndexes.begin()),
				selectedItemIndexes.end(),
				[this](auto index)
			{
				const auto& item = listViewItems.at(index);
				item->SetState(item->GetState() & ~ListViewItemState::Selected);
			});
		}

		selectionMode = _selectionMode;		
		selectionChangedEvent.Emit();
	}
	
	void ListView::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
		}

		std::ranges::for_each(listViewItems, [g](const auto& listViewItem)
		{
			listViewItem->CreateDeviceResources(g);
		});
	}
	void ListView::DiscardDeviceResources()
	{
		auto g = GetGraphics();

		backgroundBrush.ReleaseBrush();

		std::ranges::for_each(listViewItems, [g](const auto& listViewItem)
		{
			listViewItem->DiscardDeviceResources(g);
		});
	}

	std::optional<std::size_t> ListView::GetHoveredListViewItemIndex(long yPos) const noexcept
	{
		long totalHeight = 0;

		for (const auto& [index, listViewItem] : listViewItems | std::views::enumerate)
		{
			auto height = listViewItem->GetHeight();

			if (totalHeight <= yPos &&
				yPos <= totalHeight + height)
			{
				return index;
			}

			totalHeight += height;
		}

		return std::nullopt;
	}
	std::optional<std::size_t> ListView::GetSelectedItemIndex() const noexcept
	{
		if (auto iter = std::ranges::find_if(listViewItems, [](const auto& item)
		{
			return item->IsSelected();
		}); iter != listViewItems.end())
		{
			return iter - listViewItems.begin();
		}
		return std::nullopt;
	}
	std::vector<std::size_t> ListView::GetSelectedItemIndexes() const noexcept
	{
		std::vector<std::size_t> selectedItemIndexes;
		for (const auto& [index, item] : listViewItems | std::views::enumerate)
		{
			if (item->IsSelected())
			{
				selectedItemIndexes.push_back(index);
			}
		}
		return selectedItemIndexes;
	}

	bool ListView::IsIndexSelected(std::size_t index) const noexcept
	{
		if (index <= listViewItems.size())
		{
			return listViewItems.at(index)->IsSelected();
		}
		return false;
	}

	long ListView::CalculateListViewItemHeightUpToIndex(std::size_t index) const noexcept
	{
		return std::accumulate(
			listViewItems.cbegin(), std::next(listViewItems.cbegin(), index), 0,
			[](long y, const auto& item) { return y + item->GetHeight(); }
		);
	}
	long ListView::GetTotalListViewItemHeight() const noexcept
	{
		return std::accumulate(
			listViewItems.cbegin(), listViewItems.cend(), 0,
			[](long y, const auto& item) { return y + item->GetHeight(); }
		);
	}

	void ListView::Select(std::size_t index) noexcept
	{
		if (IsIndexSelected(index))
		{
			return;
		}
		switch (selectionMode)
		{
			using enum PGUI::UI::Controls::SelectionMode;
			case Single:
			{
				SelectSingle(index);
				break;
			}
			case Multiple:
			{
				SelectMultiple(index);
				break;
			}
			case Extended:
			{
				SelectExtended(index);
				break;
			}
		}
	}
	void ListView::Deselect(std::size_t index) noexcept
	{
		if (!IsIndexSelected(index))
		{
			return;
		}
		RemoveSelected(index);
		selectionChangedEvent.Emit();
	}

	void ListView::SetStateWithSelected(std::size_t index, ListViewItemState state) noexcept
	{
		if (index < listViewItems.size())
		{
			listViewItems.at(index)->SetState(state | ListViewItemState::Selected);
		}
	}
	void ListView::AddStateIfSelected(std::size_t index, ListViewItemState state) noexcept
	{
		if (index >= listViewItems.size())
		{
			return;
		}

		if (const auto& item = listViewItems.at(index);
			IsFlagSet(item->GetState(), ListViewItemState::Selected))
		{
			item->SetState(ListViewItemState::Selected | state);
		}
		else
		{
			item->SetState(state);
		}
	}

	void ListView::AddSelected(std::size_t index) noexcept
	{
		if (index < listViewItems.size())
		{
			const auto& item = listViewItems.at(index);
			item->SetState(item->GetState() | ListViewItemState::Selected);
		}
	}
	void ListView::RemoveSelected(std::size_t index) noexcept
	{
		if (index < listViewItems.size())
		{
			const auto& item = listViewItems.at(index);
			item->SetState(item->GetState() & ~ListViewItemState::Selected);
		}
	}

	void ListView::UpdateScrollBar()
	{
		auto clientRect = GetClientRect();
		auto clientSize = clientRect.Size();
		auto totalItemHeight = GetTotalListViewItemHeight();

		if (totalItemHeight <= clientSize.cy)
		{
			scrollBar->Show(SW_HIDE);
			return;
		}
		else
		{
			scrollBar->Show();
		}

		scrollBar->SetMaxScroll(static_cast<std::int64_t>(totalItemHeight - clientSize.cy));
		scrollBar->SetPageSize(clientSize.cy);
		scrollBar->SetScrollMult();
	}

	void ListView::OnScroll()
	{
		Invalidate();
	}

	void ListView::SelectSingle(std::size_t index) noexcept
	{
		if (auto prevSelected = GetSelectedItemIndex(); 
			prevSelected.has_value())
		{
			RemoveSelected(*prevSelected);
		}
		AddSelected(index);
		selectionChangedEvent.Emit();
	}
	void ListView::SelectMultiple(std::size_t index) noexcept
	{
		AddSelected(index);
		selectionChangedEvent.Emit();
	}
	void ListView::SelectExtended(std::size_t index, bool shiftPressed, bool ctrlPressed) noexcept
	{
		auto selectedItemIndexes = GetSelectedItemIndexes();

		if (!lastPressedIndex.has_value() ||
			!(shiftPressed || ctrlPressed))
		{
			std::ranges::for_each(selectedItemIndexes, [this](const auto& i)
			{
				RemoveSelected(i);
			});

			AddSelected(index);

			selectionChangedEvent.Emit();
			return;
		}

		if (shiftPressed)
		{
			if (*lastPressedIndex == index)
			{
				return;
			}

			std::ranges::for_each(selectedItemIndexes, [this, index](const auto& i)
			{
				if (i != index)
				{
					AddSelected(i);
				}
			});

			AddSelected(index);
			selectionChangedEvent.Emit();
		}
		else if (ctrlPressed)
		{
			if (IsIndexSelected(*hoveringIndex))
			{
				RemoveSelected(index);
				selectionChangedEvent.Emit();
			}
			else
			{
				AddSelected(index);
				selectionChangedEvent.Emit();
			}
		}
	}

	Core::HandlerResult ListView::OnCreate(UINT, WPARAM, LPARAM)
	{
		auto clientRect = GetClientRect();
		auto clientSize = GetClientRect().Size();
		auto totalItemHeight = GetTotalListViewItemHeight();
		
		ScrollBar::ScrollBarParams params{ totalItemHeight - clientSize.cy, 0, totalItemHeight - clientSize.cy };

		scrollBar = AddChildWindow<ScrollBar>(
			Core::WindowCreateParams{ L"ListView_ScrollBar", PointI{ clientRect.right - 20, 0 }, SizeI{ 20, clientSize.cy }, NULL },
			params
		);
		scrollBar->ScrolledEvent().Subscribe(BindMemberFunc(&ListView::OnScroll, this));
		scrollBar->Show();

		return 0;
	}

	Core::HandlerResult ListView::OnPaint(UINT, WPARAM, LPARAM)
	{
		BeginDraw();

		long totalHeight = 0;
		long width = GetClientSize().cx;
		long height = GetClientSize().cy;
		auto scrollPos = scrollBar->GetScrollPos();

		auto g = GetGraphics();

		g.Clear(backgroundBrush);
		
		g.SetTransform(
			D2D1::Matrix3x2F::Translation(
				SizeF{ 0, -static_cast<float>(scrollPos) }
			)
		);

		for (const auto& listViewItem : listViewItems)
		{
			if (totalHeight < scrollPos - height)
			{
				totalHeight += listViewItem->GetHeight();
				continue;
			}
			else if (totalHeight > scrollPos + height)
			{
				break;
			}

			auto itemRect = RectF{
				0,
				static_cast<float>(totalHeight),
				static_cast<float>(width - 20 * IsWindowVisible(scrollBar->Hwnd())),
				static_cast<float>(totalHeight + listViewItem->GetHeight())
			};

			g.PushAxisAlignedClip(itemRect, g.GetAntialiasMode());

			listViewItem->Render(g, itemRect);

			g.PopAxisAlignedClip();

			totalHeight += listViewItem->GetHeight();
		}

		g.SetTransform(D2D1::IdentityMatrix());

		EndDraw();

		return 0;
	}
	Core::HandlerResult ListView::OnMouseWheel(UINT, WPARAM wParam, LPARAM)
	{
		scrollBar->WheelScroll(GET_WHEEL_DELTA_WPARAM(wParam));

		return 0;
	}
	Core::HandlerResult ListView::OnMouseMove(UINT, WPARAM, LPARAM lParam)
	{
		TRACKMOUSEEVENT tme{ };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = Hwnd();
		TrackMouseEvent(&tme);

		PointL mousePos = MAKEPOINTS(lParam);
		mousePos.y += static_cast<long>(scrollBar->GetScrollPos());

		const auto prevHoveredItemIndex = hoveringIndex;
		hoveringIndex = GetHoveredListViewItemIndex(mousePos.y);

		if (mousePos.y > GetTotalListViewItemHeight())
		{
			if (prevHoveredItemIndex.has_value())
			{
				AddStateIfSelected(*prevHoveredItemIndex, ListViewItemState::Normal);
				Invalidate();
			}
			return 0;
		}

		hoveringIndex = GetHoveredListViewItemIndex(mousePos.y);

		if (prevHoveredItemIndex.has_value() &&
			prevHoveredItemIndex < listViewItems.size() &&
			*prevHoveredItemIndex != *hoveringIndex)
		{
			AddStateIfSelected(*prevHoveredItemIndex, ListViewItemState::Normal);
		}
		if (hoveringIndex.has_value() &&
			hoveringIndex < listViewItems.size() &&
			!IsFlagSet(listViewItems.at(*hoveringIndex)->GetState(), ListViewItemState::Pressed))
		{
			AddStateIfSelected(*hoveringIndex, ListViewItemState::Hover);
		}

		if (prevHoveredItemIndex != hoveringIndex)
		{
			Invalidate();
		}

		return 0;
	}
	Core::HandlerResult ListView::OnMouseLButtonDown(UINT, WPARAM, LPARAM)
	{
		if (hoveringIndex.has_value())
		{
			AddStateIfSelected(*hoveringIndex, ListViewItemState::Pressed);
			Invalidate();
		}

		return 0;
	}
	Core::HandlerResult ListView::OnMouseLButtonUp(UINT, WPARAM wParam, LPARAM)
	{
		if (!hoveringIndex.has_value())
		{
			return 0;
		}
		else if (const auto& item = listViewItems.at(*hoveringIndex);
			!(item->GetState() & ListViewItemState::Pressed))
		{
			return 0;
		}


		//! Abomination
		switch (selectionMode)
		{
			case PGUI::UI::Controls::SelectionMode::Single:
			{
				lastPressedIndex = *hoveringIndex;
				Select(*hoveringIndex);
				Invalidate();
				break;
			}
			case PGUI::UI::Controls::SelectionMode::Multiple:
			{
				if (IsIndexSelected(*hoveringIndex))
				{
					Deselect(*hoveringIndex);

				}
				else
				{
					Select(*hoveringIndex);
				}

				lastPressedIndex = *hoveringIndex;
				Invalidate();
				break;
			}
			case PGUI::UI::Controls::SelectionMode::Extended:
			{
				bool shiftPressed = wParam & MK_SHIFT;
				bool ctrlPressed = wParam & MK_CONTROL;

				SelectExtended(*hoveringIndex, shiftPressed, ctrlPressed);

				lastPressedIndex = *hoveringIndex;
				Invalidate();
				break;
			}
		}

		return 0;
	}
	Core::HandlerResult ListView::OnMouseLeave(UINT, WPARAM, LPARAM)
	{
		if (hoveringIndex.has_value())
		{
			AddStateIfSelected(*hoveringIndex, ListViewItemState::Normal);
			Invalidate();
		}
		hoveringIndex = std::nullopt;

		return 0;
	}
	Core::HandlerResult ListView::OnSize(UINT, WPARAM, LPARAM)
	{
		UpdateScrollBar();

		std::ranges::for_each(listViewItems, [](const auto& item)
		{
			item->OnListViewSizeChanged();
		});

		return 0;
	}

	#pragma endregion
}
