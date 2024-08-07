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
			std::int64_t minScroll;
			std::int64_t maxScroll;

			ScrollBarDirection direction;

			ScrollBarParams(
				std::int64_t pageSize,
				std::int64_t minScroll,
				std::int64_t maxScroll,
				ScrollBarDirection direction = ScrollBarDirection::Vertical) noexcept :
				pageSize{ pageSize },
				minScroll{ minScroll }, maxScroll{ maxScroll },
				direction{ direction }
			{
			}
		};

		explicit ScrollBar(const ScrollBarParams& params) noexcept;

		[[nodiscard]] std::int64_t GetPageSize() const noexcept { return pageSize; }
		[[nodiscard]] std::int64_t GetMaxScroll() const noexcept { return maxScroll; }
		[[nodiscard]] std::int64_t GetMinScroll() const noexcept { return minScroll; }
		[[nodiscard]] std::int64_t GetScrollMult() const noexcept { return scrollMult; }
		[[nodiscard]] std::int64_t GetScrollPos() const noexcept { return scrollPos; }
		[[nodiscard]] std::int64_t GetScrollRange() const noexcept { return maxScroll - minScroll; }

		void SetPageSize(std::int64_t pageSize) noexcept;
		void SetMaxScroll(std::int64_t maxScroll) noexcept;
		void SetMinScroll(std::int64_t minScroll) noexcept;
		/**
		* @brief Auto calculates scroll multiplier (ceil(maxScroll - minScroll) / 100)
		* so that it takes 100 "ticks" to scroll if that makes sense (idk how to explain)
		*/
		void SetScrollMult() noexcept;
		void SetScrollMult(std::int64_t scrollMult) noexcept;
		void SetScrollPos(std::int64_t scrollPos) noexcept;

		void ScrollTo(std::int64_t toScroll) noexcept { SetScrollPos(toScroll); }
		void ScrollRelative(std::int64_t delta) noexcept { SetScrollPos(scrollPos + delta); }

		void WheelScroll(std::int64_t wheelDelta) noexcept;

		[[nodiscard]] Core::Event<void>& ScrolledEvent() noexcept { return scrolledEvent; }

		void SetThumbBrush(Brush&  brush);
		void SetBackgroundBrush(Brush& brush);
		
		[[nodiscard]] const Brush& GetThumbBrush() const noexcept { return thumbBrush; }
		[[nodiscard]] const Brush& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		[[nodiscard]] float GetThumbPadding() const noexcept { return thumbPadding; }
		[[nodiscard]] std::pair<float, float> GetThumbRadii() const noexcept { return { thumbXRadius, thumbYRadius }; }

		[[nodiscard]] ScrollBarDirection GetDirection() const noexcept { return direction; }

		void SetThumbPadding(float padding) noexcept;
		void SetThumbRadii(std::pair<float, float> radii) noexcept;
		void SetThumbYRadius(float xRadius) noexcept;
		void SetThumbXRadius(float yRadius) noexcept;

		private:
		inline static std::int64_t buttonSize = 20;
		inline static std::int64_t minThumbHeight = 20;

		Core::WindowPtr<TextButton> upButton = nullptr;
		Core::WindowPtr<TextButton> downButton = nullptr;

		Brush thumbBrush;
		Brush backgroundBrush;

		ScrollBarDirection direction;

		std::int64_t pageSize;
		std::int64_t maxScroll;
		std::int64_t minScroll;
		std::int64_t scrollMult = 1;
		std::int64_t scrollPos;

		float thumbPos = 0;
		float instantThumbPos = 0;
		float thumbPosOffset = 0;
		bool mouseScrolling = false;

		std::int64_t wheelScrollExtra = 0;

		float thumbPadding = 0.15f;
		float thumbXRadius = 0;
		float thumbYRadius = 0;

		Core::Event<void> scrolledEvent;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		void AdjustRect(WPARAM wParam, LPRECT rc) const noexcept;

		void OnButtonClicked(bool isUp);

		[[nodiscard]] float CalculateThumbSize() const noexcept;
		[[nodiscard]] RectF CalculateThumbRect() const noexcept;
		[[nodiscard]] float CalculateThumbPos() const noexcept;
		[[nodiscard]] std::int64_t CalculateScrollPosFromThumbPos(float thumbPos) const noexcept;

		Core::HandlerResult OnDPIChange(float dpiScale, RectI suggestedRect) override;
		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnPaint(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		Core::HandlerResult OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnMouseWheel(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnWindowPosChanging(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnSizing(UINT msg, WPARAM wParam, LPARAM lParam) const;
		Core::HandlerResult OnNCCalcSize(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
