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

	enum class ListViewItemState
	{
		Normal = 0x00,
		Hover = 0x01,
		Pressed = 0x02,
		Selected = 0x04
	};
}
EnableEnumFlag(PGUI::UI::Controls::ListViewItemState)

namespace PGUI::UI::Controls
{
	class ListView;

	class ListViewItem
	{
		friend class ListView;

		public:
		virtual ~ListViewItem() = default;

		virtual void Create() = 0;

		virtual void Render(Graphics::Graphics g, RectF renderRect) = 0;
		virtual void CreateDeviceResources(Graphics::Graphics g) = 0;
		virtual void DiscardDeviceResources(Graphics::Graphics g) = 0;

		void SetState(ListViewItemState newState) noexcept { state = newState; stateChangedEvent.Emit(); }
		[[nodiscard]] auto GetState() const noexcept { return state; }

		[[nodiscard]] auto& StateChangedEvent() noexcept { return stateChangedEvent; }
		[[nodiscard]] auto& HeightChangedEvent() noexcept { return heightChangedEvent; }

		void SetHeight(long _height) noexcept { height = _height; heightChangedEvent.Emit(); }
		[[nodiscard]] auto GetHeight() const noexcept { return height; }
		[[nodiscard]] auto GetMinHeight() const noexcept { return minHeight; }

		bool IsSelected() const noexcept { return IsFlagSet(state, ListViewItemState::Selected); }

		protected:
		explicit ListViewItem(long height) noexcept :
			height{ height }
		{
		}

		[[nodiscard]] auto* GetListViewWindow() const noexcept { return listView; }
		void Invalidate() const noexcept;

		void SetMinHeight(long newMinHeight) noexcept { minHeight = newMinHeight; }

		virtual void OnDPIChanged(float dpiScale) = 0;
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
			BrushParameters normalText;
			BrushParameters normalBackground;
			BrushParameters hoverText;
			BrushParameters hoverBackground;
			BrushParameters pressedText;
			BrushParameters pressedBackground;
			BrushParameters selectedIndicator;

			ListViewTextItemColors() = default;
		};

		ListViewTextItem(std::wstring_view text, long height = 75, TextFormat textFormat = TextFormat{ }) noexcept;

		[[nodiscard]] auto GetTextLayout() const noexcept { return textLayout; }
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		void SetText(std::wstring_view _text) noexcept { text = _text; }
		[[nodiscard]] auto& GetText() const noexcept { return text; }
		[[nodiscard]] auto& GetText() noexcept { return text; }

		void SetColors(const ListViewTextItemColors& colors) noexcept;
		[[nodiscard]] auto& GetColors() const noexcept { return colors; }
		[[nodiscard]] auto& GetColors() noexcept { return colors; }

		protected:
		void Create() override;
		void Render(Graphics::Graphics g, RectF renderRect) override;
		void CreateDeviceResources(Graphics::Graphics g) override;
		void DiscardDeviceResources(Graphics::Graphics g) override;

		void OnStateChanged();
		void OnDPIChanged(float dpiScale) override;
		void OnListViewSizeChanged() override;

		private:
		std::wstring text;

		ListViewTextItemColors colors;

		TextFormat textFormat;
		TextLayout textLayout;

		Brush textBrush;
		Brush backgroundBrush;
		Brush selectedIndicatorBrush;
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
			UpdateScrollBar();
		}
		void RemoveItem(std::size_t index) noexcept;

		void Clear() noexcept;
		void ClearSelected() noexcept;
		void ClearUnselected() noexcept;

		void SelectAll() noexcept;
		void DeselectAll() noexcept;

		template <std::derived_from<ListViewItem> T>
		[[nodiscard]] auto* GetItem(std::size_t index) const
		{
			auto& ptr = listViewItems.at(index);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] auto* GetItem(std::size_t index) const
		{
			return listViewItems.at(index).get();
		}

		template <std::derived_from<ListViewItem> T>
		[[nodiscard]] auto* GetHoveredItem() const
		{
			auto& ptr = listViewItems.at(*hoveringIndex);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] auto* GetHoveredItem() const
		{
			return listViewItems.at(*hoveringIndex).get();
		}

		[[nodiscard]] auto GetSelectedItems() const noexcept -> std::vector<ListViewItem*>;

		[[nodiscard]] auto& GetListViewItems() const noexcept { return listViewItems; }
		[[nodiscard]] auto GetSelectedItemIndex() const noexcept -> std::optional<std::size_t>;
		[[nodiscard]] auto GetSelectedItemIndexes() const noexcept -> std::vector<std::size_t>;
		[[nodiscard]] auto IsIndexSelected(std::size_t index) const noexcept -> bool;

		[[nodiscard]] auto CalculateListViewItemHeightUpToIndex(std::size_t index) const noexcept -> long;
		[[nodiscard]] auto GetTotalListViewItemHeight() const noexcept -> long;

		void Select(std::size_t index) noexcept;
		void Deselect(std::size_t index) noexcept;

		[[nodiscard]] auto GetScrollBar() const noexcept { return scrollBar; }

		void SetBackgroundBrush(Brush& brush) noexcept;
		[[nodiscard]] auto& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		void SetSelectionMode(SelectionMode selectionMode) noexcept;
		[[nodiscard]] SelectionMode GetSelectionMode() const noexcept { return selectionMode; }

		[[nodiscard]] auto& SelectionChangedEvent() noexcept { return selectionChangedEvent; }
		[[nodiscard]] auto& ItemsChangedEvent() noexcept { return itemsChangedEvent; }

		private:
		Core::WindowPtr<ScrollBar> scrollBar{};

		Core::Event<void> itemsChangedEvent;
		Core::Event<void> selectionChangedEvent;

		std::optional<std::size_t> lastPressedIndex = std::nullopt;
		std::optional<std::size_t> hoveringIndex = std::nullopt;

		ListViewItemList listViewItems{ };

		Brush backgroundBrush;

		SelectionMode selectionMode = SelectionMode::Single;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		void OnClipChanged() override;

		[[nodiscard]] auto GetHoveredListViewItemIndex(long yPos) const noexcept -> std::optional<std::size_t>;

		void SetStateWithSelected(std::size_t index, ListViewItemState state) noexcept;
		void AddStateIfSelected(std::size_t index, ListViewItemState state) noexcept;
		void AddSelected(std::size_t index) noexcept;
		void RemoveSelected(std::size_t index) noexcept;

		void UpdateScrollBar();
		void OnScroll();

		void SelectSingle(std::size_t index) noexcept;
		void SelectMultiple(std::size_t index) noexcept;
		void SelectExtended(std::size_t index, bool shiftPressed = false, bool ctrlPressed = false) noexcept;

		auto OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult override;
		auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseWheel(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
	};
}
