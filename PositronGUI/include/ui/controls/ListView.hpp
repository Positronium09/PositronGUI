#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/controls/ScrollBar.hpp"
#include "ui/Brush.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"

#include <vector>


namespace PGUI::UI::Controls
{
	enum class SelectionMode
	{
		Single,
		Multiple,
		Extended
	};

	struct _listviewitem_state_flag_values
	{
		enum EnumValues
		{
			Normal = 0x00,
			Hover = 0x01,
			Pressed = 0x02,
			Selected = 0x04
		};
	};
	using ListViewItemState = EnumFlag<_listviewitem_state_flag_values>;

	class ListView;

	class ListViewItem
	{
		friend class ListView;

		public:
		virtual ~ListViewItem() = default;

		virtual void Create() = 0;
		virtual void Render(ComPtr<ID2D1DeviceContext7> dc, RectF renderRect) = 0;

		virtual void CreateDeviceResources(ComPtr<ID2D1DeviceContext7> dc) = 0;
		virtual void DiscardDeviceResources(ComPtr<ID2D1DeviceContext7> dc) = 0;

		void SetState(ListViewItemState newState) noexcept { state = newState; stateChangedEvent.Emit(); }
		[[nodiscard]] ListViewItemState GetState() const noexcept { return state; }

		[[nodiscard]] Core::Event<void>& StateChangedEvent() noexcept { return stateChangedEvent; }
		[[nodiscard]] Core::Event<void>& HeightChangedEvent() noexcept { return heightChangedEvent; }

		void SetHeight(long _height) noexcept { height = _height; heightChangedEvent.Emit(); }
		[[nodiscard]] long GetHeight() const noexcept { return height; }
		[[nodiscard]] long GetMinHeight() const noexcept { return minHeight; }

		bool IsSelected() const noexcept { return state & ListViewItemState::Selected; }

		protected:
		explicit ListViewItem(long height) noexcept :
			height{ height }
		{
		}

		[[nodiscard]] ListView* GetListViewWindow() const noexcept { return listView; }
		void Invalidate() const noexcept;

		void SetMinHeight(long newMinheight) noexcept { minHeight = newMinheight; }

		virtual void OnListViewSizeChanged() = 0;

		private:
		Core::Event<void> stateChangedEvent;
		Core::Event<void> heightChangedEvent;
		ListViewItemState state = ListViewItemState::Normal;

		long height;
		long minHeight = 20;
		ListView* listView = nullptr;
	};

	class ListViewTextItem : public ListViewItem
	{
		public:
		struct ListViewTextItemColors
		{
			BrushParameters normal;
			BrushParameters hover;
			BrushParameters pressed;
			BrushParameters selectedIndicator;
			BrushParameters text;

			ListViewTextItemColors() = default;
		};

		ListViewTextItem(std::wstring_view text, long height = 75, TextFormat textFormat = TextFormat{ }) noexcept;

		[[nodiscard]] TextLayout GetTextLayout() const noexcept { return textLayout; }
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view _text) noexcept { text = _text; }
		[[nodiscard]] const std::wstring& GetText() const noexcept { return text; }
		[[nodiscard]] std::wstring& GetText() noexcept { return text; }

		void SetColors(const ListViewTextItemColors& colors) noexcept;
		[[nodiscard]] ListViewTextItemColors GetColors() const noexcept;

		protected:
		void Create() override;
		void Render(ComPtr<ID2D1DeviceContext7> dc, RectF renderRect) override;
		void CreateDeviceResources(ComPtr<ID2D1DeviceContext7> dc) override;
		void DiscardDeviceResources(ComPtr<ID2D1DeviceContext7> dc) override;

		void OnListViewSizeChanged() override;

		private:
		std::wstring text;
		TextFormat textFormat;
		TextLayout textLayout;

		Brush normalBrush;
		Brush hoverBrush;
		Brush pressedBrush;
		Brush selectedIndicatorBrush;
		Brush textBrush;
	};

	class ListView : public Control
	{
		using ListViewItemList = std::vector<std::unique_ptr<ListViewItem>>;

		public:
		ListView() noexcept;

		template <std::derived_from<ListViewItem> T, typename ...Args>
		void AddItem(Args... args)
		{
			listViewItems.push_back(std::make_unique<T>(args...));
			listViewItems.at(listViewItems.size() - 1)->listView = this;
			listViewItems.at(listViewItems.size() - 1)->Create();
			itemsChangedEvent.Emit();
		}
		void RemoveItem(std::size_t index) noexcept;

		void Clear() noexcept;
		void ClearSelected() noexcept;
		void ClearUnselected() noexcept;

		template <std::derived_from<ListViewItem> T>
		[[nodiscard]] T* GetItem(std::size_t index) const
		{
			auto& ptr = listViewItems.at(index);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] ListViewItem* GetItem(std::size_t index) const
		{
			return listViewItems.at(index).get();
		}

		template <std::derived_from<ListViewItem> T>
		[[nodiscard]] T* GetHoveredItem() const
		{
			auto& ptr = listViewItems.at(*hoveringIndex);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] ListViewItem* GetHoveredItem() const
		{
			return listViewItems.at(*hoveringIndex).get();
		}

		[[nodiscard]] std::vector<ListViewItem*> GetSelectedItems() const noexcept;

		[[nodiscard]] const ListViewItemList& GetListViewItems() const noexcept { return listViewItems; }
		[[nodiscard]] std::optional<std::size_t> GetSelectedItemIndex() const noexcept;
		[[nodiscard]] std::vector<std::size_t> GetSelectedItemIndexes() const noexcept;
		[[nodiscard]] bool IsIndexSelected(std::size_t index) const noexcept;

		[[nodiscard]] long CalculateListViewItemHeightUpToIndex(std::size_t index) const noexcept;
		[[nodiscard]] long GetTotalListViewItemHeight() const noexcept;

		void Select(std::size_t index) noexcept;
		void Deselect(std::size_t index) noexcept;

		auto GetScrollBar() const noexcept { return scrollBar; }

		void SetBackgroundBrush(Brush& brush) noexcept;
		[[nodiscard]] const Brush& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		void SetSelectionMode(SelectionMode selectionMode) noexcept;
		[[nodiscard]] SelectionMode GetSelectionMode() const noexcept { return selectionMode; }

		Core::Event<void>& SelectionChangedEvent() noexcept { return selectionChangedEvent; }
		Core::Event<void>& ItemsChangedEvent() noexcept { return itemsChangedEvent; }

		private:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		[[nodiscard]] std::optional<std::size_t> GetHoveredListViewItemIndex(long yPos) const noexcept;

		void SetStateWithSelected(std::size_t index, ListViewItemState state) noexcept;
		void AddStateIfSelected(std::size_t index, ListViewItemState state) noexcept;
		void AddSelected(std::size_t index) noexcept;
		void RemoveSelected(std::size_t index) noexcept;

		void UpdateScrollBar();
		void OnScroll();

		void SelectSingle(std::size_t index) noexcept;
		void SelectMultiple(std::size_t index) noexcept;
		void SelectExtended(std::size_t index, bool shiftPressed = false, bool ctrlPressed = false) noexcept;

		Core::WindowPtr<ScrollBar> scrollBar;

		Core::Event<void> itemsChangedEvent;
		Core::Event<void> selectionChangedEvent;

		std::optional<std::size_t> lastPressedIndex = std::nullopt;
		std::optional<std::size_t> hoveringIndex = std::nullopt;

		ListViewItemList listViewItems;

		Brush backgroundBrush;

		SelectionMode selectionMode = SelectionMode::Single;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseWheel(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
