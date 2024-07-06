#pragma once

#include "core/Event.hpp"
#include "ui/Control.hpp"
#include "ui/Brush.hpp"
#include "TextButton.hpp"


namespace PGUI::UI::Controls
{
	enum class ScrollBarDirection
	{
		Vertical,
		Horizontal
	};

	class ScrollBar : public Control
	{
		public:
		struct ScrollBarParams
		{
			std::int64_t pageSize;
			std::int64_t lineCount;
			std::int64_t maxScroll;
			std::int64_t minScroll;

			ScrollBarDirection direction;

			ScrollBarParams(
				std::int64_t pageSize,
				std::int64_t lineCount,
				std::int64_t maxScroll,
				std::int64_t minScroll,
				ScrollBarDirection direction = ScrollBarDirection::Vertical) noexcept :
				pageSize{ pageSize }, lineCount{ lineCount }, 
				maxScroll{ maxScroll }, minScroll{ minScroll },
				direction{ direction }
			{
			}
		};

		explicit ScrollBar(const ScrollBarParams& params) noexcept;

		[[nodiscard]] std::int64_t GetPageSize() const noexcept { return pageSize; }
		[[nodiscard]] std::int64_t GetMaxScroll() const noexcept { return maxScroll; }
		[[nodiscard]] std::int64_t GetMinScroll() const noexcept { return minScroll; }
		[[nodiscard]] std::int64_t GetScrollPos() const noexcept { return scrollPos; }

		void SetPageSize(std::int64_t pageSize) noexcept;
		void SetMaxScroll(std::int64_t maxScroll) noexcept;
		void SetMinScroll(std::int64_t minScroll) noexcept;
		void SetScrollPos(std::int64_t scrollPos) noexcept;

		void ScrollTo(std::int64_t toScroll) noexcept { SetScrollPos(toScroll); }
		void ScrollRelative(std::int64_t delta) noexcept { SetScrollPos(scrollPos + delta); }

		void WheelScroll(std::int64_t wheelDelta) noexcept;

		[[nodiscard]] Core::Event<void>& GetScrolledEvent() noexcept { return scrolledEvent; }

		void SetThumbBrush(Brush&  brush);
		void SetBackgroundBrush(Brush& brush);
		
		[[nodiscard]] const Brush& GetThumbBrush() const noexcept { return thumbBrush; }
		[[nodiscard]] const Brush& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		[[nodiscard]] float GetThumbPadding() const noexcept { return thumbPadding; }
		[[nodiscard]] std::pair<float, float> GetThumbRadii() const noexcept { return { thumbXRadius, thumbYRadius }; }

		void SetThumbPadding(float padding) noexcept;
		void SetThumbRadii(std::pair<float, float> radii) noexcept;
		void SetThumbYRadius(float xRadius) noexcept;
		void SetThumbXRadius(float yRadius) noexcept;

		private:
		inline static const std::int64_t buttonSize = 20;
		inline static const std::int64_t minThumbHeight = 20;

		Core::WindowPtr<TextButton> upButton;
		Core::WindowPtr<TextButton> downButton;

		Brush thumbBrush;
		Brush backgroundBrush;

		ScrollBarDirection direction;

		std::int64_t pageSize;
		std::int64_t lineCount;
		std::int64_t maxScroll;
		std::int64_t minScroll;
		std::int64_t scrollPos = 0;

		std::int64_t wheelScrollExtra = 0;

		float thumbPadding = 0.15f;
		float thumbXRadius = 0;
		float thumbYRadius = 0;

		Core::Event<void> scrolledEvent;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		[[nodiscard]] RectF CalculateThumbRect() const noexcept;
		void AdjustRect(WPARAM wParam, LPRECT rc) const noexcept;

		void OnButtonClicked(bool isUp);

		[[nodiscard]] std::int64_t GetScrollPosFromPoint(PointI p) const noexcept;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseWheel(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnWindowPosChanging(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnSizing(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnNCCalcSize(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
