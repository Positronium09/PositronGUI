#pragma once

#include "Clip.hpp"
#include "core/DirectCompositionWindow.hpp"


namespace PGUI::UI
{
	class UIComponent : public Core::DirectCompositionWindow
	{
		public:
		explicit UIComponent(const Core::WindowClass::WindowClassPtr& wndClass) noexcept;
		UIComponent() noexcept;

		void SetClip(const ClipParameters& params) noexcept;
		void ClearClip() noexcept;
		[[nodiscard]] auto GetClip() const noexcept -> const Clip& { return clip; }
		void HitTestClipGeometry(bool enable) noexcept;

		void EnableInput() const noexcept;
		void DisableInput() const noexcept;
		[[nodiscard]] auto IsInputEnabled() const noexcept -> bool;

		protected:
		void BeginDraw() override;
		auto EndDraw() -> HRESULT override;

		virtual void OnClipChanged() { /* */ }

		private:
		Clip clip;
		bool hitTestClipGeometry = true;

		[[nodiscard]] auto OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		[[nodiscard]] auto OnNcHitTest(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept -> Core::HandlerResult;
		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept -> Core::HandlerResult;
	};
}
