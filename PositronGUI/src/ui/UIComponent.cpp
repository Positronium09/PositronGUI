#include "ui/UIComponent.hpp"
#include "ui/Colors.hpp"


namespace PGUI::UI
{
	UIComponent::UIComponent(const Core::WindowClass::WindowClassPtr& wndClass) noexcept :
		DirectCompositionWindow{ wndClass }
	{
		RegisterMessageHandler(WM_CREATE, &UIComponent::OnCreate);
		RegisterMessageHandler(WM_NCHITTEST, &UIComponent::OnNcHitTest);
		RegisterMessageHandler(WM_SIZE, &UIComponent::OnSize);
	}
	UIComponent::UIComponent() noexcept :
		UIComponent{ Core::WindowClass::Create(L"UIComponent") }
	{
	}

	void UIComponent::SetClip(const ClipParameters& params) noexcept
	{
		clip = params;
		AdjustClipForWindow(clip, this);
		OnClipChanged();
	}

	void UIComponent::ClearClip() noexcept
	{
		clip.ReleaseClip();
	}

	void UIComponent::HitTestClipGeometry(bool enable) noexcept
	{
		hitTestClipGeometry = enable;
	}

	void UIComponent::EnableInput() const noexcept
	{
		Enable(true);
	}

	void UIComponent::DisableInput() const noexcept
	{
		Enable(false);
	}

	auto UIComponent::IsInputEnabled() const noexcept -> bool
	{
		return IsWindowEnabled(Hwnd());
	}

	auto UIComponent::OnCreate(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) const noexcept -> Core::HandlerResult
	{
		/*
		* Disable input by default
		* Only controls that need input should have it enabled so that the input messages goes to the parent instead
		* which will be handy when ComponentContainer contains a window that dont use any input
		*/
		DisableInput();
		return 0;
	}

	auto UIComponent::OnNcHitTest(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult
	{
		LRESULT defResult = DefWindowProcW(Hwnd(), msg, wParam, lParam);

		if (!hitTestClipGeometry || defResult != HTCLIENT || !clip->GetClipGeometry())
		{
			return defResult;
		}

		PointL point = ScreenToClient(MAKEPOINTS(lParam));

		BOOL contains = FALSE;
		clip.Get()->GetClipGeometry()->FillContainsPoint(point, D2D1::IdentityMatrix(), &contains);

		if (!contains)
		{
			return { HTTRANSPARENT, Core::HandlerResultFlag::ForceThisResult };
		}

		return defResult;
	}

	auto UIComponent::OnSize(UINT /*unused*/, WPARAM /*unused*/, LPARAM /*unused*/) noexcept -> Core::HandlerResult
	{
		AdjustClipForWindow(clip, this);
		return 0;
	}

	void UIComponent::BeginDraw()
	{
		DirectCompositionWindow::BeginDraw();
		auto g = GetGraphics();

		auto prevTransform = g.GetTransform();
		g.SetTransform(GetDpiScaleTransform());

		g.Clear(Colors::Transparent);

		g.SetTransform(prevTransform);

		g.PushLayer(D2D1::LayerParameters(
			D2D1::InfiniteRect(), clip.Get()->GetClipGeometryPtr()
		), nullptr);
	}

	auto UIComponent::EndDraw() -> HRESULT
	{
		GetGraphics().PopLayer();
	
		return DirectCompositionWindow::EndDraw();
	}
}
