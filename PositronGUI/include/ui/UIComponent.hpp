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
		const Clip& GetClip() const noexcept { return clip; }
		void HitTestClipGeometry(bool enable) noexcept;

		void EnableInput() const noexcept;
		void DisableInput() const noexcept;
		[[nodiscard]] bool IsInputEnabled() const noexcept;

		protected:
		void BeginDraw() override;
		HRESULT EndDraw() override;

		private:
		Clip clip;
		bool hitTestClipGeometry = true;

		Core::HandlerResult OnCreate(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnNcHitTest(UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};
}
