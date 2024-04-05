#pragma once

#include "Window.hpp"
#include "helpers/ComPtr.hpp"

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_6.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dcomp.h>


namespace PGUI::Core
{
	class DirectCompositionWindow : public Window
	{
		public:
		explicit DirectCompositionWindow(const WindowClass::WindowClassPtr& wndClass) noexcept;

		protected:
		[[nodiscard]] ComPtr<ID2D1DeviceContext7> GetRenderingInterface() const noexcept;

		[[nodiscard]] ComPtr<ID3D11Device2> D3D11Device() const noexcept;
		[[nodiscard]] ComPtr<IDXGIDevice4> DXGIDevice() const noexcept;
		[[nodiscard]] ComPtr<IDXGISwapChain1> DXGISwapChain() const noexcept;
		[[nodiscard]] ComPtr<IDCompositionDevice> DCompositionDevice() const noexcept;
		[[nodiscard]] ComPtr<IDCompositionTarget> DCompositionTarget() const noexcept;
		[[nodiscard]] ComPtr<ID2D1Device7> D2D1Device() const noexcept;
		[[nodiscard]] ComPtr<ID2D1DeviceContext7> D2D1DeviceContext() const noexcept;

		virtual void BeginDraw();
		virtual HRESULT EndDraw();

		virtual void CreateDeviceResources();
		virtual void DiscardDeviceResources();

		private:
		ComPtr<ID3D11Device2> d3d11Device;
		ComPtr<IDXGIDevice4> dxgiDevice;
		ComPtr<IDXGISwapChain1> swapChain;
		ComPtr<IDCompositionDevice> dcompDevice;
		ComPtr<IDCompositionTarget> dcompTarget;
		ComPtr<ID2D1Device7> d2d1Device;
		ComPtr<ID2D1DeviceContext7> d2d1Dc;

		void InitD3D11Device();
		void InitD2D1Device();
		void InitD2D1DeviceContextTarget() const;
		void InitDirectComposition();

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
