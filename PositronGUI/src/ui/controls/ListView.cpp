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

		DiscardDeviceResources(nullptr);
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
	void ListViewTextItem::Render(ComPtr<ID2D1DeviceContext7> dc, RectF renderRect)
	{
		ListViewItemState currentState = GetState();

		switch (currentState)
		{
			using enum ListViewItemState::EnumValues;

			case Normal:
			case Selected:
			{
				dc->FillRectangle(renderRect, normalBrush->GetBrushPtr());
				break;
			}
			case Hover:
			case Selected | Hover:
			{
				dc->FillRectangle(renderRect, hoverBrush->GetBrushPtr());
				break;
			}
			case Pressed:
			case Selected | Pressed:
			{
				dc->FillRectangle(renderRect, pressedBrush->GetBrushPtr());
				break;
			}
			default:
				break;
		}

		if (currentState & ListViewItemState::Selected)
		{
			auto selectedIndicatorRect = renderRect;
			selectedIndicatorRect.top += 10;
			selectedIndicatorRect.bottom -= 10;
			selectedIndicatorRect.left -= 5;
			selectedIndicatorRect.right = selectedIndicatorRect.left + 10;

			dc->FillRoundedRectangle(RoundedRect{ selectedIndicatorRect, 3.5f, 5 }, selectedIndicatorBrush->GetBrushPtr());
		}

		dc->DrawTextLayout(renderRect.TopLeft(), textLayout, textBrush->GetBrushPtr());
	}
	void ListViewTextItem::CreateDeviceResources(ComPtr<ID2D1DeviceContext7> dc)
	{
		if (!normalBrush)
		{
			SetGradientBrushRect(normalBrush, GetListViewWindow()->GetClientRect());
			normalBrush.CreateBrush(dc);
		}

		if (!hoverBrush)
		{
			SetGradientBrushRect(hoverBrush, GetListViewWindow()->GetClientRect());
			hoverBrush.CreateBrush(dc);
		}

		if (!pressedBrush)
		{
			SetGradientBrushRect(pressedBrush, GetListViewWindow()->GetClientRect());
			pressedBrush.CreateBrush(dc);
		}

		if (!textBrush)
		{
			SetGradientBrushRect(normalBrush, textLayout.GetBoundingRect());
			textBrush.CreateBrush(dc);
		}
		if (!selectedIndicatorBrush)
		{
			selectedIndicatorBrush.CreateBrush(dc);
		}
	}
	void ListViewTextItem::DiscardDeviceResources(ComPtr<ID2D1DeviceContext7>)
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
		if (std::ranges::contains(selectedItemIndexes, index))
		{
			const auto found = std::ranges::find(selectedItemIndexes, index);
			selectedItemIndexes.erase(found);
		}
		listViewItems.erase(std::ranges::next(listViewItems.begin(), index));
	}
	
	std::vector<ListViewItem*> ListView::GetSelectedItems() const noexcept
	{
		std::vector<ListViewItem*> selectedItems{ selectedItemIndexes.size() };

		std::ranges::for_each(selectedItemIndexes, [this, &selectedItems](const auto& index)
		{
			selectedItems.push_back(listViewItems.at(index).get());
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

		if (selectionMode == SelectionMode::Single &&
			!selectedItemIndexes.empty())
		{
			std::ranges::for_each(
				std::ranges::next(selectedItemIndexes.begin()),
				selectedItemIndexes.end(),
				[this](auto index)
			{
				const auto& item = listViewItems.at(index);
				item->SetState(item->GetState() & ~ListViewItemState::Selected);
			});

			selectedItemIndexes.erase(
				std::ranges::next(selectedItemIndexes.begin()),
				selectedItemIndexes.end()
			);
		}

		selectionMode = _selectionMode;
		
		if (auto prevSelectionSize = selectedItemIndexes.size(); 
			prevSelectionSize != selectedItemIndexes.size())
		{
			selectionChangedEvent.Emit();
		}
	}
	
	void ListView::CreateDeviceResources()
	{
		auto renderer = GetRenderingInterface();

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			backgroundBrush.CreateBrush(renderer);
		}

		std::ranges::for_each(listViewItems, [renderer](const auto& listViewItem)
		{
			listViewItem->CreateDeviceResources(renderer);
		});
	}
	void ListView::DiscardDeviceResources()
	{
		auto renderer = GetRenderingInterface();

		backgroundBrush.ReleaseBrush();

		std::ranges::for_each(listViewItems, [renderer](const auto& listViewItem)
		{
			listViewItem->DiscardDeviceResources(renderer);
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

	void ListView::SetStateWithSelected(std::size_t index, ListViewItemState state) noexcept
	{
		listViewItems.at(index)->SetState(state | ListViewItemState::Selected);
	}
	void ListView::AddStateIfSelected(std::size_t index, ListViewItemState state) noexcept
	{
		if (const auto& item = listViewItems.at(index);
			item->GetState() & ListViewItemState::Selected)
		{
			item->SetState(ListViewItemState::Selected | state);
		}
		else
		{
			item->SetState(state);
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
		scrollBar->SetPageSize(scrollBar->GetMaxScroll() / scrollBar->GetLineCount());
	}

	void ListView::OnScroll()
	{
		Invalidate();
	}

	Core::HandlerResult ListView::OnCreate(UINT, WPARAM, LPARAM)
	{
		auto clientRect = GetClientRect();
		auto clientSize = GetClientRect().Size();
		auto totalItemHeight = GetTotalListViewItemHeight();
		
		ScrollBar::ScrollBarParams params{ (totalItemHeight - clientSize.cy) / 10,
			10, totalItemHeight - clientSize.cy, 0 };

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

		auto renderer = GetRenderingInterface();

		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());
		
		renderer->SetTransform(
			D2D1::Matrix3x2F::Translation(
				SizeF{ 0, -static_cast<float>(scrollBar->GetScrollPos()) }
			)
		);

		for (const auto& listViewItem : listViewItems)
		{
			auto itemRect = RectF{
				0,
				static_cast<float>(totalHeight),
				static_cast<float>(width - 20 * IsWindowVisible(scrollBar->Hwnd())),
				static_cast<float>(totalHeight + listViewItem->GetHeight())
			};

			renderer->PushAxisAlignedClip(itemRect, renderer->GetAntialiasMode());

			listViewItem->Render(renderer, itemRect);

			renderer->PopAxisAlignedClip();

			totalHeight += listViewItem->GetHeight();
		}

		renderer->SetTransform(D2D1::IdentityMatrix());

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
			~(listViewItems.at(*hoveringIndex)->GetState() & ListViewItemState::Pressed))
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
				if (auto selectedIndex = GetSelectedItemIndex();
					selectedIndex.has_value())
				{
					const auto found = std::ranges::find(selectedItemIndexes, *selectedIndex);
					selectedItemIndexes.erase(found);

					if (*selectedIndex == *hoveringIndex)
					{
						listViewItems.at(*selectedIndex)->SetState(ListViewItemState::Hover);
					}
					else
					{
						listViewItems.at(*selectedIndex)->SetState(ListViewItemState::Normal);
						SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
						selectedItemIndexes.push_back(*hoveringIndex);
					}

					lastPressedIndex = *hoveringIndex;
					selectionChangedEvent.Emit();
					Invalidate();
				}
				else
				{
					selectedItemIndexes.push_back(*hoveringIndex);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);

					lastPressedIndex = *hoveringIndex;
					selectionChangedEvent.Emit();
					Invalidate();
				}
				break;
			}
			case PGUI::UI::Controls::SelectionMode::Multiple:
			{
				if (std::ranges::contains(selectedItemIndexes, *hoveringIndex))
				{
					const auto found = std::ranges::find(selectedItemIndexes, *hoveringIndex);
					selectedItemIndexes.erase(found);
					listViewItems.at(*hoveringIndex)->SetState(ListViewItemState::Hover);

					lastPressedIndex = *hoveringIndex;
					selectionChangedEvent.Emit();
					Invalidate();
				}
				else
				{
					selectedItemIndexes.push_back(*hoveringIndex);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);

					lastPressedIndex = *hoveringIndex;
					selectionChangedEvent.Emit();
					Invalidate();
				}

				break;
			}
			case PGUI::UI::Controls::SelectionMode::Extended:
			{
				bool shiftPressed = wParam & MK_SHIFT;
				bool ctrlPressed = wParam & MK_CONTROL;

				if (!lastPressedIndex.has_value() ||
					(!shiftPressed && !ctrlPressed))
				{
					std::ranges::for_each(selectedItemIndexes, [this](const auto& index)
					{
						listViewItems.at(index)->SetState(ListViewItemState::Normal);
					});

					if (!selectedItemIndexes.empty())
					{
						selectedItemIndexes.clear();
					}

					selectedItemIndexes.push_back(*hoveringIndex);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
					
					selectionChangedEvent.Emit();
					lastPressedIndex = *hoveringIndex;
					Invalidate();
					return 0;
				}

				if (shiftPressed)
				{
					if (*lastPressedIndex == *hoveringIndex)
					{
						return 0;
					}

					std::ranges::iota_view<std::size_t, std::size_t> addRange;

					if (lastPressedIndex > hoveringIndex)
					{
						addRange = std::views::iota(*hoveringIndex, *lastPressedIndex + 1);
					}
					else
					{
						addRange = std::views::iota(*lastPressedIndex, *hoveringIndex + 1);
					}

					std::ranges::for_each(selectedItemIndexes, 
						[this, &addRange](const auto& index)
					{
						if (!std::ranges::contains(addRange, index))
						{
							listViewItems.at(index)->SetState(ListViewItemState::Normal);
						}
					});
					selectedItemIndexes.clear();
					selectedItemIndexes.append_range(addRange);

					std::ranges::for_each(selectedItemIndexes, [this](const auto& index)
					{
						if (index != *hoveringIndex)
						{
							SetStateWithSelected(index, ListViewItemState::Normal);
						}
					});

					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
				}
				else if (ctrlPressed)
				{
					if (std::ranges::contains(selectedItemIndexes, *hoveringIndex))
					{
						const auto found = std::ranges::find(selectedItemIndexes, *hoveringIndex);
						selectedItemIndexes.erase(found);

						listViewItems.at(*hoveringIndex)->SetState(ListViewItemState::Hover);
					}
					else
					{
						selectedItemIndexes.push_back(*hoveringIndex);

						SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
					}
				}

				lastPressedIndex = *hoveringIndex;
				selectionChangedEvent.Emit();
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
