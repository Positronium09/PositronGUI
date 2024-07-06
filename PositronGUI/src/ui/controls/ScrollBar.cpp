#include "ui/controls/ScrollBar.hpp"

#include "ui/UIColors.hpp"

#include <cfenv>
#include <algorithm>

#undef max
#undef min

namespace PGUI::UI::Controls
{
	ScrollBar::ScrollBar(const ScrollBarParams& params) noexcept :
		Control{ Core::WindowClass::Create(L"ScrollBar_UIControl") },
		direction{ params.direction },
		pageSize{ params.pageSize }, lineCount{ params.lineCount }, 
		maxScroll{ params.maxScroll }, minScroll{ params.minScroll }
	{
		RegisterMessageHandler(WM_CREATE, &ScrollBar::OnCreate);
		RegisterMessageHandler(WM_PAINT, &ScrollBar::OnPaint);
		RegisterMessageHandler(WM_LBUTTONDOWN, &ScrollBar::OnLButtonDown);
		RegisterMessageHandler(WM_MOUSEMOVE, &ScrollBar::OnMouseMove);
		RegisterMessageHandler(WM_MOUSEWHEEL, &ScrollBar::OnMouseWheel);
		RegisterMessageHandler(WM_MOUSEHWHEEL, &ScrollBar::OnMouseWheel);
		RegisterMessageHandler(WM_WINDOWPOSCHANGING, &ScrollBar::OnWindowPosChanging);
		RegisterMessageHandler(WM_SIZING, &ScrollBar::OnSizing);
		RegisterMessageHandler(WM_NCCALCSIZE, &ScrollBar::OnNCCalcSize);

		if (auto uiColors = UIColors::GetInstance();
			uiColors->IsDarkMode())
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
		if (_pageSize < maxScroll - minScroll)
		{
			pageSize = _pageSize;
		}
	}
	void ScrollBar::SetMaxScroll(std::int64_t _maxScroll) noexcept
	{
		maxScroll = _maxScroll;
	}
	void ScrollBar::SetMinScroll(std::int64_t _minScroll) noexcept
	{
		minScroll = _minScroll;
	}
	void ScrollBar::SetScrollPos(std::int64_t _scrollPos) noexcept
	{
		scrollPos = std::clamp(_scrollPos, minScroll, maxScroll);

		Invalidate();
		scrolledEvent.Emit();
	}

	void ScrollBar::WheelScroll(std::int64_t wheelDelta) noexcept
	{
		std::int64_t scroll = [this]() -> std::int64_t
		{
			UINT scrollParam = 0;
			if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollParam, 0))
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
		ScrollRelative(-lines);
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
		auto renderer = GetRenderingInterface();

		if (!thumbBrush)
		{
			SetGradientBrushRect(thumbBrush, CalculateThumbRect());
			thumbBrush.CreateBrush(renderer);
		}

		if (!backgroundBrush)
		{
			SetGradientBrushRect(backgroundBrush, GetClientRect());
			backgroundBrush.CreateBrush(renderer);
		}
	}
	void ScrollBar::DiscardDeviceResources()
	{
		thumbBrush.ReleaseBrush();
		backgroundBrush.ReleaseBrush();
	}

	RectF ScrollBar::CalculateThumbRect() const noexcept
	{
		RectF clientRect = GetClientRect();
		auto size = clientRect.Size();

		if (direction == ScrollBarDirection::Vertical)
		{
			size.cy -= buttonSize * 2;

			auto thumbHeight = std::clamp(
				static_cast<float>(pageSize)
				/ static_cast<float>(maxScroll - minScroll)
				* size.cy, static_cast<float>(minThumbHeight), size.cy);

			const auto paddingVal = size.cx * thumbPadding;

			RectF thumbRect{
				paddingVal, paddingVal,
				size.cx * (1.0f - thumbPadding), thumbHeight - paddingVal
			};

			thumbRect.Shift(0, static_cast<float>(scrollPos)
				/ static_cast<float>(maxScroll - minScroll)
				* (size.cy - thumbHeight) + buttonSize);

			return thumbRect;
		}

		size.cx -= buttonSize * 2;

		auto thumbWidth = std::clamp(
			static_cast<float>(pageSize)
			/ static_cast<float>(maxScroll - minScroll)
			* size.cx, static_cast<float>(minThumbHeight), size.cx);

		const auto paddingVal = size.cy * thumbPadding;

		RectF thumbRect{
			paddingVal, paddingVal,
			thumbWidth - paddingVal, size.cy * (1.0f - thumbPadding)
		};

		thumbRect.Shift(static_cast<float>(scrollPos)
			/ static_cast<float>(maxScroll - minScroll)
			* (size.cx - thumbWidth) + buttonSize,
			0);

		return thumbRect;
	}

	void ScrollBar::AdjustRect(WPARAM wParam, LPRECT rc) const noexcept
	{
		RECT rect;
		int cyClient;
		int cyAdjust;
		SetRectEmpty(&rect);
		::AdjustWindowRect(&rect, 
			static_cast<DWORD>(GetWindowLongPtrW(Hwnd(), GWL_STYLE)), FALSE);
		cyClient = (rc->bottom - rc->top) - (rect.bottom - rect.top);
		cyAdjust = cyClient % pageSize;
		/*
		 *  Remove the fractional pixels from the top or bottom.
		 */
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
		auto line = (maxScroll - minScroll) / lineCount;

		if (isUp)
		{
			ScrollRelative(-line);
		}
		else
		{
			ScrollRelative(line);
		}
	}

	std::int64_t ScrollBar::GetScrollPosFromPoint(PointI p) const noexcept
	{
		const SizeF size = GetClientSize();
		PointF fp = p;

		const auto prevRoundingMode = std::fegetround();
		std::fesetround(FE_TONEAREST);

		std::int64_t ret = 0;

		if (direction == ScrollBarDirection::Vertical)
		{
			ret = static_cast<std::int64_t>(
				std::nearbyintf(
					MapToRange(fp.y,
						static_cast<float>(minScroll),
						static_cast<float>(maxScroll),
						static_cast<float>(buttonSize),
						size.cy - static_cast<float>(buttonSize)
						)
				)
				);
		}
		else
		{
			ret = static_cast<std::int64_t>(
				std::nearbyintf(
					MapToRange(fp.x,
						static_cast<float>(minScroll),
						static_cast<float>(maxScroll),
						static_cast<float>(buttonSize),
						size.cx - static_cast<float>(buttonSize)
					)
				)
				);
		}

		std::fesetround(prevRoundingMode);

		return ret;
	}

	Core::HandlerResult ScrollBar::OnCreate(UINT, WPARAM, LPARAM)
	{
		auto size = GetClientSize();

		auto colors = TextButton::GetTextButtonColors();

		colors.normalBackground = backgroundBrush.GetParameters();
		
		colors.hoverBackground = backgroundBrush.GetParameters();
		std::get<RGBA>(colors.hoverBackground).Lighten(0.02f);

		colors.clickedBackground = backgroundBrush.GetParameters();
		std::get<RGBA>(colors.clickedBackground).Darken(0.02f);

		colors.normalText = thumbBrush.GetParameters();
		colors.hoverText = thumbBrush.GetParameters();
		colors.clickedText = thumbBrush.GetParameters();

		Core::WindowCreateParams upButtonParams{ L"▲", { 0, 0 }, { size.cx, buttonSize }, NULL };
		Core::WindowCreateParams downButtonParams{ L"▼", { 0, size.cy - buttonSize }, { size.cx, buttonSize }, NULL };

		if (direction == ScrollBarDirection::Horizontal)
		{
			downButtonParams = Core::WindowCreateParams{ L"▶", { size.cx - buttonSize, 0 }, { buttonSize, size.cy }, NULL };
			upButtonParams = Core::WindowCreateParams{ L"◀", { 0, 0 }, { buttonSize, size.cy }, NULL };
		}

		upButton = AddChildWindow<TextButton>(
			upButtonParams,
			colors
		);
		downButton = AddChildWindow<TextButton>(
			downButtonParams,
			colors
		);
		upButton->SetTextFormat(TextFormat::GetDefTextFormat(12));
		downButton->SetTextFormat(TextFormat::GetDefTextFormat(12));

		upButton->ClickedEvent().Subscribe(std::bind_front(&ScrollBar::OnButtonClicked, this, true));
		downButton->ClickedEvent().Subscribe(std::bind_front(&ScrollBar::OnButtonClicked, this, false));

		upButton->Show();
		downButton->Show();

		return 0;
	}

	Core::HandlerResult ScrollBar::OnPaint(UINT, WPARAM, LPARAM)
	{
		BeginDraw();

		auto renderer = GetRenderingInterface();

		renderer->FillRectangle(GetClientRect(), backgroundBrush->GetBrushPtr());
		renderer->FillRoundedRectangle(
			RoundedRect{ CalculateThumbRect(), thumbXRadius, thumbYRadius }, thumbBrush->GetBrushPtr());

		EndDraw();
		
		return 0;
	}

	Core::HandlerResult ScrollBar::OnLButtonDown(UINT, WPARAM, LPARAM lParam)
	{
		PointI pos = MAKEPOINTS(lParam);

		SetScrollPos(GetScrollPosFromPoint(pos));

		return 0;
	}
	Core::HandlerResult ScrollBar::OnMouseMove(UINT, WPARAM wParam, LPARAM lParam)
	{
		PointI pos  MAKEPOINTS(lParam);

		if (wParam & MK_LBUTTON)
		{
			SetScrollPos(GetScrollPosFromPoint(pos));
		}

		return 0;
	}

	Core::HandlerResult ScrollBar::OnMouseWheel(UINT msg, WPARAM wParam, LPARAM)
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
	
	Core::HandlerResult ScrollBar::OnWindowPosChanging(UINT, WPARAM, LPARAM lParam) const
	{
		if (auto winPos = std::bit_cast<LPWINDOWPOS>(lParam); 
			!(winPos->flags & SWP_NOSIZE))
		{
			RECT rc = { 0, 0, winPos->cx, winPos->cy };
			AdjustRect(WMSZ_BOTTOM, &rc);
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
	Core::HandlerResult ScrollBar::OnSizing(UINT, WPARAM wParam, LPARAM lParam) const
	{
		AdjustRect(wParam, std::bit_cast<LPRECT>(lParam));
		return 0;
	}
	Core::HandlerResult ScrollBar::OnNCCalcSize(UINT msg, WPARAM wParam, LPARAM lParam)
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
