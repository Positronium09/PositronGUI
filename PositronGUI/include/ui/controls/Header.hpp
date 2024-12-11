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
		[[nodiscard]] auto GetState() const noexcept -> HeaderItemState { return state; }

		[[nodiscard]] auto StateChangedEvent() noexcept -> Core::Event<void>& { return stateChangedEvent; }
		[[nodiscard]] auto WidthChangedEvent() noexcept -> Core::Event<void>& { return widthChangedEvent; }

		void SetWidth(long _width) noexcept { width = _width; widthChangedEvent.Emit(); }
		[[nodiscard]] auto GetWidth() const noexcept -> long { return width; }
		[[nodiscard]] auto GetMinWidth() const noexcept -> long { return minWidth; }

		protected:
		explicit HeaderItem(long width) noexcept :
			width{ width }
		{
		}

		[[nodiscard]] auto GetHeaderWindow() const noexcept -> Header* { return header; }
		void Invalidate() const noexcept;

		void SetMinWidth(long newMinWidth) noexcept { minWidth = newMinWidth; }

		virtual void OnDPIChanged(float dpiScale) = 0;
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

		[[nodiscard]] static auto GetHeaderTextItemColors() noexcept -> HeaderTextItemColors;
		[[nodiscard]] static auto GetHeaderTextItemAccentedColors() noexcept -> HeaderTextItemColors;

		HeaderTextItem(std::wstring_view text, long width, 
			HeaderTextItemColors  colors = GetHeaderTextItemColors(), 
			TextFormat tf = TextFormat{ }) noexcept;

		[[nodiscard]] auto GetColors() const noexcept -> const HeaderTextItemColors& { return colors; }
		[[nodiscard]] auto GetColors() noexcept -> HeaderTextItemColors& { return colors; }

		[[nodiscard]] auto GetTextLayout() const noexcept -> TextLayout { return textLayout; }
		void SetTextFormat(TextFormat textFormat) noexcept;

		void InitTextLayout();

		protected:
		void Create() override;
		void Render(Graphics::Graphics g, RectF renderRect) override;
		void CreateDeviceResources(Graphics::Graphics g) override;
		void DiscardDeviceResources(Graphics::Graphics /*g*/) override;

		void OnDPIChanged(float dpiScale) override;
		void OnHeaderSizeChanged() override;

		private:
		std::wstring text;
		
		HeaderTextItemColors colors;

		TextFormat textFormat;
		TextLayout textLayout;

		Brush textBrush;
		Brush backgroundBrush;

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
		[[nodiscard]] auto GetItem(std::size_t index) const -> T*
		{
			const auto& ptr = headerItems.at(index);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] auto GetItem(std::size_t index) const -> HeaderItem*
		{
			return headerItems.at(index).get();
		}

		template <std::derived_from<HeaderItem> T>
		[[nodiscard]] auto GetHoveredItem() const -> T*
		{
			const auto& ptr = headerItems.at(*hoveringIndex);

			return dynamic_cast<T*>(ptr.get());
		}
		[[nodiscard]] auto GetHoveredItem() const -> HeaderItem*
		{
			return headerItems.at(*hoveringIndex).get();
		}

		[[nodiscard]] auto GetHeaderItems() const noexcept -> const HeaderItemList& { return headerItems; }

		void SetSeparatorBrush(const Brush& separatorBrush) noexcept;
		void SetBackgroundBrush(const Brush& backgroundBrush) noexcept;

		[[nodiscard]] auto HeaderItemClickedEvent() -> Core::Event<std::size_t>& { return headerItemClickedEvent; }

		private:
		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		[[nodiscard]] auto GetHoveredHeaderItemIndex(long xPos) const noexcept -> std::optional<std::size_t>;
		[[nodiscard]] auto IsMouseOnSeparator(long xPos) const noexcept -> bool;

		[[nodiscard]] auto CalculateHeaderItemWidthUpToIndex(std::size_t index) const noexcept -> long;
		[[nodiscard]] auto GetTotalHeaderWidth() const noexcept -> long;

		Core::Event<std::size_t> headerItemClickedEvent;

		static inline const long sizingMargin = 5;
		HeaderItemList headerItems;

		Brush separatorBrush;
		Brush backgroundBrush;

		bool dragging = false;
		bool mouseOnDivider = false;

		std::optional<std::size_t> hoveringIndex = std::nullopt;

		auto OnDPIChange(float dpiScale, RectI suggestedRect) noexcept -> Core::HandlerResult override;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		[[nodiscard]] auto OnSetCursor(UINT msg, WPARAM wParam, LPARAM lParam) const -> Core::HandlerResult;
		[[nodiscard]] auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const -> Core::HandlerResult;
	};
}
