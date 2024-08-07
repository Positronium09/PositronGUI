#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/Brush.hpp"
#include "ui/TextFormat.hpp"
#include "ui/TextLayout.hpp"

#include <optional>


namespace PGUI::UI::Controls
{
	class Header;

	enum class HeaderItemState
	{
		Normal,
		Hover,
		Pressed
	};

	class HeaderItem
	{
		friend class Header;

		public:
		virtual ~HeaderItem() = default;

		virtual void Create() = 0;
		virtual void Render(Graphics::Graphics g, RectF renderRect) = 0;

		virtual void CreateDeviceResources(Graphics::Graphics g) = 0;
		virtual void DiscardDeviceResources(Graphics::Graphics g) = 0;

		void SetState(HeaderItemState newState) noexcept { state = newState; stateChangedEvent.Emit(); }
		[[nodiscard]] HeaderItemState GetState() const noexcept { return state; }

		[[nodiscard]] Core::Event<void>& GetStateChangedEvent() noexcept { return stateChangedEvent; }
		[[nodiscard]] Core::Event<void>& GetWidthChangedEvent() noexcept { return widthChangedEvent; }

		void SetWidth(long _width) noexcept { width = _width; widthChangedEvent.Emit(); }
		[[nodiscard]] long GetWidth() const noexcept { return width; }
		[[nodiscard]] long GetMinWidth() const noexcept { return minWidth; }

		protected:
		explicit HeaderItem(long width) noexcept :
			width{ width }
		{
		}

		[[nodiscard]] Header* GetHeaderWindow() const noexcept { return header; }
		void Invalidate() const noexcept;

		void SetMinWidth(long newMinWidth) noexcept { minWidth = newMinWidth; }

		virtual void OnHeaderSizeChanged() = 0;

		private:
		Core::Event<void> stateChangedEvent;
		Core::Event<void> widthChangedEvent;
		HeaderItemState state = HeaderItemState::Normal;

		long width;
		long minWidth = 20;
		Header* header = nullptr;
	};

	class HeaderTextItem : public HeaderItem
	{
		public:
		struct HeaderTextItemColors
		{
			BrushParameters normalText;
			BrushParameters normalBackground;
			BrushParameters hoverText;
			BrushParameters hoverBackground;
			BrushParameters pressedText;
			BrushParameters pressedBackground;

			HeaderTextItemColors() = default;
		};

		[[nodiscard]] static HeaderTextItemColors GetHeaderTextItemColors() noexcept;
		[[nodiscard]] static HeaderTextItemColors GetHeaderTextItemAccentedColors() noexcept;

		HeaderTextItem(std::wstring_view text, long width, 
			const HeaderTextItemColors& colors = GetHeaderTextItemColors(), 
			TextFormat tf = TextFormat::GetDefTextFormat()) noexcept;

		[[nodiscard]] const HeaderTextItemColors& GetColors() const noexcept { return colors; }
		[[nodiscard]] HeaderTextItemColors& GetColors() noexcept { return colors; }

		protected:
		void Create() override;
		void Render(Graphics::Graphics g, RectF renderRect) override;
		void CreateDeviceResources(Graphics::Graphics g) override;
		void DiscardDeviceResources(Graphics::Graphics) override;

		void OnHeaderSizeChanged() override;

		private:
		std::wstring text;
		
		HeaderTextItemColors colors;

		TextFormat textFormat;
		TextLayout textLayout;

		Brush textBrush;
		Brush backgroundBrush;

		void InitTextLayout();

		void OnStateChanged() noexcept;

	};

	class Header : public Control
	{
		using HeaderItemList = std::vector<std::unique_ptr<HeaderItem>>;

		public:
		Header();

		template <std::derived_from<HeaderItem> T, typename ...Args>
		void AddItem(Args... args)
		{
			headerItems.push_back(std::make_unique<T>(args...));
			headerItems.at(headerItems.size() - 1)->header = this;
			headerItems.at(headerItems.size() - 1)->Create();
		}
		
		template <std::derived_from<HeaderItem> T>
		[[nodiscard]] T* GetItem(std::size_t index) const
		{
			auto& ptr = headerItems.at(index);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] HeaderItem* GetItem(std::size_t index) const
		{
			return headerItems.at(index).get();
		}

		template <std::derived_from<HeaderItem> T>
		[[nodiscard]] T* GetHoveredItem() const
		{
			auto& ptr = headerItems.at(*hoveringIndex);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] HeaderItem* GetHoveredItem() const
		{
			return headerItems.at(*hoveringIndex).get();
		}

		[[nodiscard]] const HeaderItemList& GetHeaderItems() const noexcept { return headerItems; }

		void SetSeperatorBrush(const Brush& seperatorBrush) noexcept;
		void SetBackgroundBrush(const Brush& backgroundBrush) noexcept;

		[[nodiscard]] Core::Event<std::size_t>& HeaderItemClickedEvent() { return headerItemClickedEvent; }

		private:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		[[nodiscard]] std::optional<std::size_t> GetHoveredHeaderItemIndex(long xPos) const noexcept;
		[[nodiscard]] bool IsMouseOnSeperator(long xPos) const noexcept;

		[[nodiscard]] long CalculateHeaderItemWidthUpToIndex(std::size_t index) const noexcept;
		[[nodiscard]] long GetTotalHeaderWidth() const noexcept;

		Core::Event<std::size_t> headerItemClickedEvent;

		const long sizingMargin = 5;
		HeaderItemList headerItems;

		Brush seperatorBrush;
		Brush backgroundBrush;

		bool dragging = false;
		bool mouseOnDivider = false;

		std::optional<std::size_t> hoveringIndex = std::nullopt;

		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSetCursor(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const;
	};
}
