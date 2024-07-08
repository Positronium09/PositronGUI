#include "ui/controls/ListView.hpp"

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

	ListViewTextItem::ListViewTextItem() noexcept : 
		ListViewItem{ 100 }
	{
	}

	void ListViewTextItem::Create()
	{
		normal.SetParameters(RGBA{ 0xffffff });
		hover.SetParameters(RGBA{ 0x0000ff });
		pressed.SetParameters(RGBA{ 0x00ff00 });
		selected.SetParameters(RGBA{ 0xff0000 });
		selectedHover.SetParameters(RGBA{ 0xffff00 });
		selectedPressed.SetParameters(RGBA{ 0xff00ff });
	}
	void ListViewTextItem::Render(ComPtr<ID2D1DeviceContext7> dc, RectF renderRect)
	{
		ListViewItemState s = GetState();

		switch (s)
		{
			using enum ListViewItemState::EnumValues;

			case Normal:
			{
				dc->DrawRectangle(renderRect, normal->GetBrushPtr(), 5.0f);
				break;
			}
			case Hover:
			{
				dc->DrawRectangle(renderRect, hover->GetBrushPtr(), 5.0f);
				break;
			}
			case Pressed:
			{
				dc->DrawRectangle(renderRect, pressed->GetBrushPtr(), 5.0f);
				break;
			}
			case Selected:
			{
				dc->DrawRectangle(renderRect, selected->GetBrushPtr(), 5.0f);
				break;
			}
			case Selected | Hover:
			{
				dc->DrawRectangle(renderRect, selectedHover->GetBrushPtr(), 5.0f);
				break;
			}
			case Selected | Pressed:
			{
				dc->DrawRectangle(renderRect, selectedPressed->GetBrushPtr(), 5.0f);
				break;
			}
			default:
				break;
		}
	}
	void ListViewTextItem::CreateDeviceResources(ComPtr<ID2D1DeviceContext7> dc)
	{
		normal.CreateBrush(dc);
		hover.CreateBrush(dc);
		pressed.CreateBrush(dc);
		selected.CreateBrush(dc);
		selectedHover.CreateBrush(dc);
		selectedPressed.CreateBrush(dc);
	}
	void ListViewTextItem::DiscardDeviceResources(ComPtr<ID2D1DeviceContext7> dc)
	{
		normal.ReleaseBrush();
		hover.ReleaseBrush();
		pressed.ReleaseBrush();
		selected.ReleaseBrush();
		selectedHover.ReleaseBrush();
		selectedPressed.ReleaseBrush();
	}

	#pragma endregion

	#pragma region ListView

	ListView::ListView() noexcept :
		Control{ Core::WindowClass::Create(L"ListView_UIControl") }
	{
		RegisterMessageHandler(WM_PAINT, &ListView::OnPaint);
		RegisterMessageHandler(WM_MOUSEMOVE, &ListView::OnMouseMove);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ListView::OnMouseLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &ListView::OnMouseLButtonUp);
		RegisterMessageHandler(WM_MOUSELEAVE, &ListView::OnMouseLeave);

		backgroundBrush.SetParameters(RGBA{ 0 });

		listViewItems.push_back(std::make_unique<ListViewTextItem>());
		listViewItems.push_back(std::make_unique<ListViewTextItem>());
		listViewItems.push_back(std::make_unique<ListViewTextItem>());
		listViewItems.push_back(std::make_unique<ListViewTextItem>());
		listViewItems.push_back(std::make_unique<ListViewTextItem>());
		listViewItems.push_back(std::make_unique<ListViewTextItem>());

		for (const auto& item : listViewItems)
		{
			item->Create();
		}
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

		if (selectionMode == SelectionMode::Single)
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
	long ListView::CalculateHeaderItemHeightUpToIndex(std::size_t index) const noexcept
	{
		return std::accumulate(
			listViewItems.cbegin(), std::next(listViewItems.cbegin(), index), 0,
			[](long y, const auto& item) { return y + item->GetHeight(); }
		);
	}
	long ListView::GetTotalHeaderHeight() const noexcept
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

	Core::HandlerResult ListView::OnPaint(UINT, WPARAM, LPARAM)
	{
		BeginDraw();

		long totalHeight = 0;
		long width = GetClientSize().cx;

		auto renderer = GetRenderingInterface();

		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());

		for (const auto& listViewItem : listViewItems)
		{
			auto itemRect = RectF{
				0,
				static_cast<float>(totalHeight),
				static_cast<float>(width),
				static_cast<float>(totalHeight + listViewItem->GetHeight())
			};

			renderer->PushAxisAlignedClip(itemRect, renderer->GetAntialiasMode());

			listViewItem->Render(renderer, itemRect);

			renderer->PopAxisAlignedClip();

			totalHeight += listViewItem->GetHeight();
		}

		EndDraw();

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

		const auto prevHoveredItemIndex = hoveringIndex;
		hoveringIndex = GetHoveredListViewItemIndex(mousePos.y);

		if (mousePos.y > GetTotalHeaderHeight())
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
					auto [begin, end] = std::ranges::remove(selectedItemIndexes, *selectedIndex);
					std::ranges::for_each(begin, end, [this](const auto& index)
					{
						listViewItems.at(index)->SetState(ListViewItemState::Normal);
					});
					selectedItemIndexes.erase(begin, end);

					listViewItems.at(*selectedIndex)->SetState(ListViewItemState::Normal);
					
					selectedItemIndexes.push_back(*hoveringIndex);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);

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
					auto [begin, end] = std::ranges::remove(selectedItemIndexes, *hoveringIndex);
					std::ranges::for_each(begin, end, [this](const auto& index)
					{
						listViewItems.at(index)->SetState(ListViewItemState::Normal);
					});
					selectedItemIndexes.erase(begin, end);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);

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
					selectedItemIndexes.erase(selectedItemIndexes.end());


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
						addRange = std::views::iota(*hoveringIndex, *lastPressedIndex);
					}
					else
					{
						addRange = std::views::iota(*hoveringIndex, *lastPressedIndex);
					}

					std::ranges::for_each(selectedItemIndexes, [this](const auto& index)
					{
						listViewItems.at(index)->SetState(ListViewItemState::Normal);
					});
					selectedItemIndexes.erase(selectedItemIndexes.end());

					selectedItemIndexes.append_range(addRange);
					std::ranges::for_each(selectedItemIndexes, [this](const auto& index)
					{
						SetStateWithSelected(index, ListViewItemState::Normal);
					});

					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
				}
				else if (ctrlPressed)
				{
					if (*lastPressedIndex == *hoveringIndex)
					{
						auto [begin, end] = std::ranges::remove(selectedItemIndexes, *hoveringIndex);
						std::ranges::for_each(begin, end, [this](const auto& index)
						{
							listViewItems.at(index)->SetState(ListViewItemState::Normal);
						});
						selectedItemIndexes.erase(begin, end);
						
						listViewItems.at(*hoveringIndex)->SetState(ListViewItemState::Hover);

						selectionChangedEvent.Emit();

						lastPressedIndex = *hoveringIndex;
						return 0;
					}

					selectedItemIndexes.push_back(*hoveringIndex);
					SetStateWithSelected(*hoveringIndex, ListViewItemState::Hover);
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

	#pragma endregion
}
