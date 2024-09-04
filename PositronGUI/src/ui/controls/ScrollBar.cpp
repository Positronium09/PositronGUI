#include "ui/controls/ScrollBar.hpp"

#include "ui/UIColors.hpp"
#include "helpers/ScopedTimer.hpp"

#include <cfenv>
#include <algorithm>


namespace PGUI::UI::Controls
{
	ScrollBar::ScrollBar(const ScrollBarParams& params) noexcept :
		Control{ Core::WindowClass::Create(L"ScrollBar_UIControl") },
		direction{ params.direction },
		pageSize{ params.pageSize }, 
		maxScroll{ params.maxScroll }, minScroll{ params.minScroll },
		scrollMult{ static_cast<std::int64_t>(
			std::ceil(
				static_cast<double>(params.maxScroll - params.minScroll) / 100.0
			)) },
		scrollPos{ params.minScroll }
	{
		RegisterMessageHandler(WM_CREATE, &ScrollBar::OnCreate);
		RegisterMessageHandler(WM_PAINT, &ScrollBar::OnPaint);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ScrollBar::OnLButtonDown);
		RegisterMessageHandler(WM_LBUTTONUP, &ScrollBar::OnLButtonUp);
		RegisterMessageHandler(WM_MOUSEMOVE, &ScrollBar::OnMouseMove);
		RegisterMessageHandler(WM_MOUSEWHEEL, &ScrollBar::OnMouseWheel);
		RegisterMessageHandler(WM_MOUSEHWHEEL, &ScrollBar::OnMouseWheel);
		RegisterMessageHandler(WM_WINDOWPOSCHANGING, &ScrollBar::OnWindowPosChanging);
		RegisterMessageHandler(WM_SIZING, &ScrollBar::OnSizing);
		RegisterMessageHandler(WM_NCCALCSIZE, &ScrollBar::OnNCCalcSize);

		if (UIColors::IsDarkMode())
		{
			thumbBrush.SetParameters(RGBA{ 0x555555 });
			backgroundBrush.SetParameters(RGBA{ 0x181818 });
		}
		else
		{
			thumbBrush.SetParameters(RGBA{ 0x808080 });
			backgroundBrush.SetParameters(RGBA{ 0xF3F3F3 });
		}
	}

	void ScrollBar::SetPageSize(std::int64_t _pageSize) noexcept
	{
		if (_pageSize < GetScrollRange())
		{
			pageSize = _pageSize;
			Invalidate();
		}
	}
	void ScrollBar::SetMaxScroll(std::int64_t _maxScroll) noexcept
	{
		maxScroll = _maxScroll;
		Invalidate();
	}
	void ScrollBar::SetMinScroll(std::int64_t _minScroll) noexcept
	{
		minScroll = _minScroll;
		Invalidate();
	}
	void ScrollBar::SetScrollMult(std::int64_t _scrollMult) noexcept
	{
		scrollMult = std::clamp(_scrollMult, 1LL, std::numeric_limits<std::int64_t>::max());
	}
	void ScrollBar::SetScrollMult() noexcept
	{
		scrollMult = static_cast<std::int64_t>(std::ceil(static_cast<double>(GetScrollRange()) / 100.0));
	}
	void ScrollBar::SetScrollPos(std::int64_t _scrollPos) noexcept
	{
		scrollPos = std::clamp(_scrollPos, minScroll, maxScroll);
		thumbPos = CalculateThumbPos();

		Invalidate();
		scrolledEvent.Emit();
	}

	void ScrollBar::WheelScroll(std::int64_t wheelDelta) noexcept
	{
		std::int64_t scroll = [this]() -> std::int64_t
		{
			UINT scrollParam = 0;
			if (!SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &scrollParam, 0))
			{
				return 3;
			}
			if (scrollParam == WHEEL_PAGESCROLL)
			{
				return pageSize;
			}
			return static_cast<std::int64_t>(scrollParam);
		}();

		if (scroll == 0)
		{
			return;
		}

		wheelDelta += wheelScrollExtra;
		
		auto lines = wheelDelta * scroll / WHEEL_DELTA;
		
		wheelScrollExtra = wheelDelta - lines * WHEEL_DELTA / scroll;
		ScrollRelative(-lines * scrollMult);
	}

	void ScrollBar::SetThumbBrush(Brush& brush)
	{
		thumbBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}
	void ScrollBar::SetBackgroundBrush(Brush& brush)
	{
		backgroundBrush.SetParameters(brush.GetParameters());
		Invalidate();
	}

	void ScrollBar::SetThumbPadding(float padding) noexcept
	{
		thumbPadding = padding;
		Invalidate();
	}
	void ScrollBar::SetThumbRadii(std::pair<float, float> radii) noexcept
	{
		thumbXRadius = std::get<0>(radii);
		thumbYRadius = std::get<1>(radii);

		Invalidate();
	}

	void ScrollBar::SetThumbYRadius(float xRadius) noexcept
	{
		thumbXRadius = xRadius;
		Invalidate();
	}
	void ScrollBar::SetThumbXRadius(float yRadius) noexcept
	{
		thumbYRadius = yRadius;
		Invalidate();
	}

	void ScrollBar::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!thumbBrush)
		{
			SetGradientBrushRect(thumbBrush, CalculateThumbRect());
			g.CreateBrush(thumbBrush);
		}

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			g.CreateBrush(backgroundBrush);
		}
	}
	void ScrollBar::DiscardDeviceResources()
	{
		thumbBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	void ScrollBar::OnClipChanged()
	{
		upButton->SetClip(GetClip().GetParameters());
		downButton->SetClip(GetClip().GetParameters());
	}

	void ScrollBar::AdjustRect(WPARAM wParam, LPRECT rc) const noexcept
	{
		RECT rect;
		int cyClient = 0;
		int cyAdjust = 0;
		SetRectEmpty(&rect);
		::AdjustWindowRect(&rect, 
			static_cast<DWORD>(GetWindowLongPtrW(Hwnd(), GWL_STYLE)), FALSE);
		cyClient = (rc->bottom - rc->top) - (rect.bottom - rect.top);
		cyAdjust = cyClient % pageSize;

		switch (wParam)
		{
			case WMSZ_TOP:
			case WMSZ_TOPLEFT:
			case WMSZ_TOPRIGHT:
				rc->top += cyAdjust;
				break;
			default:
				rc->bottom -= cyAdjust;
				break;
		}
	}

	void ScrollBar::OnButtonClicked(bool isUp)
	{
		if (isUp)
		{
			ScrollRelative(-scrollMult);
		}
		else
		{
			ScrollRelative(scrollMult);
		}
	}

	auto ScrollBar::CalculateThumbSize() const noexcept -> float
	{
		SizeF size = GetClientSize();
		if (direction == ScrollBarDirection::Vertical)
		{
			size.cy -= static_cast<float>(buttonSize) * 2;
			if (size.cy < static_cast<float>(minThumbHeight)) //* bandage
			{
				return 1;
			}

			auto thumbHeight = std::clamp(
				static_cast<float>(pageSize)
				/ static_cast<float>(GetScrollRange() + pageSize)
				* size.cy, static_cast<float>(minThumbHeight), size.cy);

			return thumbHeight;
		}

		size.cx -= static_cast<float>(buttonSize) * 2;
		if (size.cx < static_cast<float>(minThumbHeight)) //* bandage
		{
			return 1;
		}

		auto thumbWidth = std::clamp(
			static_cast<float>(pageSize)
			/ static_cast<float>(GetScrollRange())
			* size.cx, static_cast<float>(minThumbHeight), size.cx);

		return thumbWidth;

	}

	auto ScrollBar::CalculateThumbRect() const noexcept -> RectF
	{
		RectF clientRect = GetClientRect();
		auto size = clientRect.Size();

		float rectShift = mouseScrolling ? instantThumbPos - thumbPosOffset : thumbPos;

		if (direction == ScrollBarDirection::Vertical)
		{
			size.cy -= ScaleByDPI(static_cast<float>(buttonSize) * 2);

			auto thumbHeight = CalculateThumbSize();
			const auto padding = ScaleByDPI(size.cx * thumbPadding);

			return RectF{
				padding, padding,
				size.cx - padding, thumbHeight - padding,
			}.Shifted(0, rectShift);
		}

		size.cx -= ScaleByDPI(static_cast<float>(buttonSize) * 2);

		auto thumbWidth = CalculateThumbSize();
		const auto padding = ScaleByDPI(size.cy * thumbPadding);

		return RectF{
			padding, padding,
			thumbWidth - padding, size.cy - padding,
		}.Shifted(rectShift, 0);
	}
	auto ScrollBar::CalculateThumbPos() const noexcept -> float
	{
		if (direction == ScrollBarDirection::Vertical)
		{
			return MapToRange(static_cast<float>(scrollPos),
				static_cast<float>(buttonSize),
				static_cast<float>(GetClientSize().cy - buttonSize) - CalculateThumbSize(),
				static_cast<float>(minScroll),
				static_cast<float>(maxScroll)
			);
		}
		return MapToRange(static_cast<float>(scrollPos),
			static_cast<float>(buttonSize),
			static_cast<float>(GetClientSize().cx - buttonSize) - CalculateThumbSize(),
			static_cast<float>(minScroll),
			static_cast<float>(maxScroll)
		);
	}
	auto ScrollBar::CalculateScrollPosFromThumbPos(float pos) const noexcept -> std::int64_t
	{
		auto size = GetClientSize();

		const auto prevRoundingMode = std::fegetround();
		std::fesetround(FE_TONEAREST);

		std::int64_t ret = 0;

		if (direction == ScrollBarDirection::Vertical)
		{
			ret = static_cast<std::int64_t>(
				std::nearbyintf(
					MapToRange(pos,
						static_cast<float>(minScroll),
						static_cast<float>(maxScroll),
						static_cast<float>(buttonSize),
						static_cast<float>(size.cy - buttonSize) - CalculateThumbSize()
					)
				)
				);
		}
		else
		{
			ret = static_cast<std::int64_t>(
				std::nearbyintf(
					MapToRange(pos,
						static_cast<float>(minScroll),
						static_cast<float>(maxScroll),
						static_cast<float>(buttonSize),
						static_cast<float>(size.cx - buttonSize) - CalculateThumbSize()
					)
				)
				);
		}

		std::fesetround(prevRoundingMode);

		return ret;
	}

	auto ScrollBar::OnDPIChange(float dpiScale, RectI suggestedRect) -> Core::HandlerResult
	{
		minThumbHeight = static_cast<std::int64_t>(static_cast<float>(minThumbHeight) * dpiScale);
		thumbPos *= dpiScale;
		instantThumbPos *= dpiScale;
		thumbPosOffset *= dpiScale;

		return Window::OnDPIChange(dpiScale, suggestedRect);
	}

	auto ScrollBar::OnCreate(UINT, WPARAM, LPARAM) -> Core::HandlerResult
	{
		thumbPos = CalculateThumbPos();

		auto size = GetClientSizeWithoutDPI();

		auto colors = TextButton::GetTextButtonColors();

		colors.normalBackground = backgroundBrush.GetParameters();
		
		colors.hoverBackground = backgroundBrush.GetParameters();
		std::get<RGBA>(colors.hoverBackground).Lighten(0.02f);

		colors.clickedBackground = backgroundBrush.GetParameters();
		std::get<RGBA>(colors.clickedBackground).Darken(0.02f);

		colors.normalText = thumbBrush.GetParameters();
		colors.hoverText = thumbBrush.GetParameters();
		colors.clickedText = thumbBrush.GetParameters();
		
		Core::WindowCreateParams upButtonParams{ L"▲", { 0, 0 }, { size.cx, static_cast<int>(buttonSize) }, NULL };
		Core::WindowCreateParams downButtonParams{ L"▼", 
			{ 0, size.cy - static_cast<int>(buttonSize) }, { size.cx, static_cast<int>(buttonSize) }, NULL };

		if (direction == ScrollBarDirection::Horizontal)
		{
			downButtonParams = Core::WindowCreateParams{ L"▶", 
				{ size.cx - static_cast<int>(buttonSize), 0 }, { static_cast<int>(buttonSize), size.cy }, NULL };
			upButtonParams = Core::WindowCreateParams{ L"◀", { 0, 0 }, 
				{ static_cast<int>(buttonSize), size.cy }, NULL };
		}

		upButton = AddChildWindow<TextButton>(
			upButtonParams,
			colors
		);

		downButton = AddChildWindow<TextButton>(
			downButtonParams,
			colors
		);
		upButton->SetTextFormat(TextFormat::GetDefTextFormat(ScaleByDPI(12.0f)));
		downButton->SetTextFormat(TextFormat::GetDefTextFormat(ScaleByDPI(12.0f)));

		upButton->ClickedEvent().Subscribe(std::bind_front(&ScrollBar::OnButtonClicked, this, true));
		downButton->ClickedEvent().Subscribe(std::bind_front(&ScrollBar::OnButtonClicked, this, false));

		upButton->Show();
		downButton->Show();

		return 0;
	}

	auto ScrollBar::OnPaint(UINT, WPARAM, LPARAM) -> Core::HandlerResult
	{
		BeginDraw();

		auto g = GetGraphics();

		g.Clear(backgroundBrush);

		auto thumbRect = CalculateThumbRect();

		g.FillRoundedRect(
			RoundedRect{ thumbRect, thumbXRadius, thumbYRadius }, thumbBrush);

		EndDraw();
		
		return 0;
	}

	auto ScrollBar::OnLButtonDown(UINT, WPARAM, LPARAM lParam) noexcept -> Core::HandlerResult
	{
		PointF p = MAKEPOINTS(lParam);
		if (!CalculateThumbRect().IsPointInside(p))
		{
			if (direction == ScrollBarDirection::Vertical)
			{
				SetScrollPos(CalculateScrollPosFromThumbPos(p.y - CalculateThumbSize() / 2.0f));
			}
			else
			{
				SetScrollPos(CalculateScrollPosFromThumbPos(p.x - CalculateThumbSize() / 2.0f));
			}
			return 0;
		}

		SetCapture(Hwnd());

		instantThumbPos = thumbPos;
		if (direction == ScrollBarDirection::Vertical)
		{
			instantThumbPos = p.y;
		}
		else
		{
			instantThumbPos = p.x;
		}
		thumbPosOffset = instantThumbPos - thumbPos;

		return 0;
	}
	auto ScrollBar::OnLButtonUp(UINT, WPARAM, LPARAM) noexcept -> Core::HandlerResult
	{
		if (mouseScrolling)
		{
			SetScrollPos(CalculateScrollPosFromThumbPos(instantThumbPos - thumbPosOffset));
			mouseScrolling = false;
			ReleaseCapture();
			return 0;
		}

		return 0;
	}
	auto ScrollBar::OnMouseMove(UINT, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult
	{
		if (!(wParam & MK_LBUTTON) || GetCapture() != Hwnd())
		{
			return 0;
		}

		mouseScrolling = true;

		PointF p = MAKEPOINTS(lParam);
		SizeF size = GetClientSize();

		if (direction == ScrollBarDirection::Vertical)
		{
			instantThumbPos = std::clamp(p.y, 
				static_cast<float>(buttonSize) + thumbPosOffset, 
				size.cy - static_cast<float>(buttonSize) - CalculateThumbSize() + thumbPosOffset);
		}
		else
		{
			instantThumbPos = std::clamp(p.x,
				static_cast<float>(buttonSize) + thumbPosOffset,
				size.cx - static_cast<float>(buttonSize) - CalculateThumbSize() + thumbPosOffset);
		}

		if (auto scroll = CalculateScrollPosFromThumbPos(instantThumbPos - thumbPosOffset);
			scroll != scrollPos)
		{
			SetScrollPos(scroll);
		}
		else
		{
			Invalidate();
		}

		return 0;
	}

	auto ScrollBar::OnMouseWheel(UINT msg, WPARAM wParam, LPARAM) -> Core::HandlerResult
	{
		const bool shouldScrollVertical = 
			msg == WM_MOUSEWHEEL && 
			~(LOWORD(wParam) & MK_SHIFT) &&
			direction == ScrollBarDirection::Vertical;
		const bool shouldScrollHorizontal = 
			(msg == WM_MOUSEHWHEEL || 
				(msg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_SHIFT)))
			&& direction == ScrollBarDirection::Horizontal;

		if (auto shouldScroll = shouldScrollVertical || shouldScrollHorizontal;
			shouldScroll)
		{
			WheelScroll(GET_WHEEL_DELTA_WPARAM(wParam));
		}

		return 0;
	}
	
	auto ScrollBar::OnWindowPosChanging(UINT, WPARAM, LPARAM lParam) const -> Core::HandlerResult
	{
		if (auto winPos = std::bit_cast<LPWINDOWPOS>(lParam); 
			!(winPos->flags & SWP_NOSIZE))
		{
			RECT rc = { 0, 0, winPos->cx, winPos->cy };
			//AdjustRect(WMSZ_BOTTOM, &rc);
			if (direction == ScrollBarDirection::Vertical)
			{
				winPos->cy = rc.bottom;
			}
			else
			{
				winPos->cx = rc.right;
			}
		}

		return 0;
	}
	auto ScrollBar::OnSizing(UINT, WPARAM wParam, LPARAM lParam) const -> Core::HandlerResult
	{
		UNREFERENCED_PARAMETER(wParam);
		UNREFERENCED_PARAMETER(lParam);
		//AdjustRect(wParam, std::bit_cast<LPRECT>(lParam));
		return 0;
	}
	auto ScrollBar::OnNCCalcSize(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult
	{
		auto ret = DefWindowProcW(Hwnd(), msg, wParam, lParam);

		auto params = std::bit_cast<LPNCCALCSIZE_PARAMS>(lParam);

		auto clampScroll = [this](std::int64_t pos)
		{
			pos = std::max(pos, 0LL);
			pos = std::min(pos, maxScroll 
				- pageSize);

			return pos;
		};

		if (!wParam)
		{
			return ret;
		}

		RECT const* prcClientNew = &params->rgrc[0];
		LPRECT prcValidDst = &params->rgrc[1];
		RECT const* prcValidSrc = &params->rgrc[2];

		std::int64_t dpos = static_cast<int64_t>(prcClientNew->top) - prcValidSrc->top;

		if (std::int64_t pos = clampScroll(minScroll + dpos); 
			direction == ScrollBarDirection::Vertical &&
			prcClientNew->bottom == prcValidSrc->bottom &&
			pageSize &&
			dpos / pageSize != 0 &&
			pos != minScroll)
		{
			*prcValidDst = *prcClientNew;

			ScrollTo(pos);

			prcValidDst->top -= static_cast<LONG>(dpos * pageSize);
			ret = WVR_VALIDRECTS;
		}
		else if (direction == ScrollBarDirection::Horizontal &&
			prcClientNew->right == prcValidSrc->right &&
			pageSize &&
			dpos / pageSize != 0 &&
			pos != minScroll)
		{
			*prcValidDst = *prcClientNew;

			ScrollTo(pos);

			prcValidDst->left -= static_cast<LONG>(dpos * pageSize);
			ret = WVR_VALIDRECTS;
		}

		return ret;
	}
}
