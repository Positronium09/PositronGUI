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

		[[nodiscard]] auto GetPageSize() const noexcept { return pageSize; }
		[[nodiscard]] auto GetMaxScroll() const noexcept { return maxScroll; }
		[[nodiscard]] auto GetMinScroll() const noexcept { return minScroll; }
		[[nodiscard]] auto GetScrollMult() const noexcept { return scrollMult; }
		[[nodiscard]] auto GetScrollPos() const noexcept { return scrollPos; }
		[[nodiscard]] auto GetScrollRange() const noexcept { return maxScroll - minScroll; }

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

		[[nodiscard]] auto& ScrolledEvent() noexcept { return scrolledEvent; }

		void SetThumbBrush(Brush&  brush);
		void SetBackgroundBrush(Brush& brush);
		
		[[nodiscard]] auto& GetThumbBrush() const noexcept { return thumbBrush; }
		[[nodiscard]] auto& GetBackgroundBrush() const noexcept { return backgroundBrush; }

		[[nodiscard]] auto GetThumbPadding() const noexcept { return thumbPadding; }
		[[nodiscard]] auto GetThumbRadii() const noexcept -> std::pair<float, float> { return { thumbXRadius, thumbYRadius }; }

		[[nodiscard]] auto GetDirection() const noexcept { return direction; }

		void SetThumbPadding(float padding) noexcept;
		void SetThumbRadii(std::pair<float, float> radii) noexcept;
		void SetThumbYRadius(float xRadius) noexcept;
		void SetThumbXRadius(float yRadius) noexcept;

		private:
		static inline const std::int64_t buttonSize = 20;
		static inline std::int64_t minThumbHeight = 20;

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

		float thumbPadding = 0.15F;
		float thumbXRadius = 0;
		float thumbYRadius = 0;

		Core::Event<void> scrolledEvent;

		void CreateDeviceResources() override;
		void DiscardDeviceResources() override;

		void OnClipChanged() override;

		void AdjustRect(WPARAM wParam, LPRECT rc) const noexcept;

		void OnButtonClicked(bool isUp);

		[[nodiscard]] auto CalculateThumbSize() const noexcept -> float;
		[[nodiscard]] auto CalculateThumbRect() const noexcept -> RectF;
		[[nodiscard]] auto CalculateThumbPos() const noexcept -> float;
		[[nodiscard]] auto CalculateScrollPosFromThumbPos(float thumbPos) const noexcept -> std::int64_t;

		auto OnDPIChange(float dpiScale, RectI suggestedRect) -> Core::HandlerResult override;
		auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnPaint(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnLButtonDown(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnLButtonUp(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
		auto OnMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnMouseWheel(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		[[nodiscard]] auto OnWindowPosChanging(UINT msg, WPARAM wParam, LPARAM lParam) const -> Core::HandlerResult;
		[[nodiscard]] static auto OnSizing(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnNCCalcSize(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
	};
}
