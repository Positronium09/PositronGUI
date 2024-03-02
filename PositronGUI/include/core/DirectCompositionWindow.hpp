#pragma once

#include "Window.hpp"
#include "helpers/ComPtr.hpp"
#include "ui/ClipGeometry.hpp"

#include <dxgi.h>
#include <dxgi1_3.h>
#include <d2d1.h>
#include <d2d1_2.h>
#include <d3d11.h>
#include <dcomp.h>


namespace PGUI::Core
{
	class DirectCompositionWindow : public Window
	{
		public:
		explicit DirectCompositionWindow(const WindowClass::WindowClassPtr& wndClass);

		protected:
		[[nodiscard]] ComPtr<ID2D1DeviceContext> GetRenderingInterface() const;

		[[nodiscard]] ComPtr<ID3D11Device> D3D11Device() const;
		[[nodiscard]] ComPtr<IDXGIDevice> DXGIDevice() const;
		[[nodiscard]] ComPtr<IDXGISwapChain1> DXGISwapChain() const;
		[[nodiscard]] ComPtr<IDCompositionDevice> DCompositionDevice() const;
		[[nodiscard]] ComPtr<IDCompositionTarget> DCompositionTarget() const;
		[[nodiscard]] ComPtr<ID2D1Device1> D2D1Device() const;
		[[nodiscard]] ComPtr<ID2D1DeviceContext> D2D1DeviceContext() const;

		virtual void BeginDraw();
		virtual HRESULT EndDraw();

		virtual void CreateDeviceResources();
		virtual void DiscardDeviceResources();

		private:
		ComPtr<ID3D11Device> d3d11Device;
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<IDXGISwapChain1> swapChain;
		ComPtr<IDCompositionDevice> dcompDevice;
		ComPtr<IDCompositionTarget> dcompTarget;
		ComPtr<ID2D1Device1> d2d1Device;
		ComPtr<ID2D1DeviceContext> d2d1Dc;

		void InitD3D11Device();
		void InitD2D1Device();
		void InitD2D1DeviceContextTarget() const;
		void InitDirectComposition();

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam) const;
	};
}
