#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/Brush.hpp"

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

		protected:
		explicit ListViewItem(long height) noexcept :
			height{ height }
		{
		}

		[[nodiscard]] ListView* GetListViewWindow() const noexcept { return listView; }
		void Invalidate() const noexcept;

		void SetMinHeight(long newMinheight) noexcept { minHeight = newMinheight; }

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
		ListViewTextItem() noexcept;

		private:
		Brush normal;
		Brush hover;
		Brush pressed;
		Brush selected;
		Brush selectedHover;
		Brush selectedPressed;

		void Create() override;
		void Render(ComPtr<ID2D1DeviceContext7> dc, RectF renderRect) override;
		void CreateDeviceResources(ComPtr<ID2D1DeviceContext7> dc) override;
		void DiscardDeviceResources(ComPtr<ID2D1DeviceContext7> dc) override;
	};

	class ListView : public Control
	{
		using ListViewItemList = std::vector<std::unique_ptr<ListViewItem>>;

		public:
		ListView() noexcept;

		[[nodiscard]] std::span<const std::size_t> GetSelectedItemIndexes() const noexcept { return selectedItemIndexes; }
		[[nodiscard]] std::optional<std::size_t> GetSelectedItemIndex() const noexcept
		{
			try
			{
				return selectedItemIndexes.at(0);

			}
			catch (std::out_of_range&)
			{
				return std::nullopt;
			}
		}

		void SetBackgroundBrush(Brush& brush) noexcept;
		[[nodiscard]] const Brush& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		void SetSelectionMode(SelectionMode selectionMode) noexcept;
		[[nodiscard]] SelectionMode GetSelectionMode() const noexcept { return selectionMode; }

		Core::Event<void> SelectionChangedEvent() const noexcept { return selectionChangedEvent; }

		private:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		[[nodiscard]] std::optional<std::size_t> GetHoveredListViewItemIndex(long yPos) const noexcept;

		[[nodiscard]] long CalculateHeaderItemHeightUpToIndex(std::size_t index) const noexcept;
		[[nodiscard]] long GetTotalHeaderHeight() const noexcept;

		void SetStateWithSelected(std::size_t index, ListViewItemState state) noexcept;
		void AddStateIfSelected(std::size_t index, ListViewItemState state) noexcept;

		Core::Event<void> selectionChangedEvent;

		std::optional<std::size_t> lastPressedIndex = std::nullopt;
		std::optional<std::size_t> hoveringIndex = std::nullopt;
		std::vector<std::size_t> selectedItemIndexes;

		ListViewItemList listViewItems;

		Brush backgroundBrush;

		SelectionMode selectionMode = SelectionMode::Single;

		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
