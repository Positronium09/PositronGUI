#include "ui/UIComponent.hpp"


namespace PGUI::UI
{
	UIComponent::UIComponent(const Core::WindowClass::WindowClassPtr& wndClass) : 
		DirectCompositionWindow{ wndClass }
	{
		RegisterMessageHandler(WM_NCHITTEST, &UIComponent::OnNcHitTest);
	}
	UIComponent::UIComponent() : 
		UIComponent{ Core::WindowClass::Create(L"UIComponent") }
	{
	}

	void UIComponent::SetClipGeometry(UI::Clip _clip)
	{
		clip = _clip;
	}

	void UIComponent::ClearClipGeometry()
	{
		clip = Clip::CreateEmptyClip();
	}

	void UIComponent::HitTestClipGeometry(bool enable)
	{
		hitTestClipGeometry = enable;
	}

	Style& PGUI::UI::UIComponent::GetStyle()
	{
		return style;
	}

	const Style& PGUI::UI::UIComponent::GetStyle() const
	{
		return style;
	}

	void PGUI::UI::UIComponent::SetStyle(const Style& _style)
	{
		style = _style;
	}

	Core::HandlerResult UIComponent::OnNcHitTest(UINT msg, WPARAM wParam, LPARAM lParam) const
	{
		LRESULT defResult = DefWindowProcW(Hwnd(), msg, wParam, lParam);
		if (!hitTestClipGeometry || clip.GetClipType() != ClipType::Empty || defResult != HTCLIENT)
		{
			return defResult;
		}

		PointL point = ScreenToClient(MAKEPOINTS(lParam));

		BOOL contains = FALSE;
		clip.GetClipGeometry()->FillContainsPoint(point, D2D1::IdentityMatrix(), &contains);

		if (!contains)
		{
			return { HTTRANSPARENT, Core::HandlerResultFlags::ForceThisResult };
		}

		return defResult;
	}

	void UIComponent::BeginDraw()
	{
		DirectCompositionWindow::BeginDraw();
		GetRenderingInterface()->PushLayer(D2D1::LayerParameters1(
			D2D1::InfiniteRect(), clip.GetClipGeometry().Get()
		), nullptr);
	}

	HRESULT UIComponent::EndDraw()
	{
		GetRenderingInterface()->PopLayer();
		return DirectCompositionWindow::EndDraw();
	}
}
