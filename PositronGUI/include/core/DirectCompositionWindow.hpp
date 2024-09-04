#pragma once

#include "Window.hpp"
#include "helpers/ComPtr.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/RenderTarget.hpp"

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_6.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dcomp.h>


namespace PGUI
{
	void Initialize();
}

namespace PGUI::Core
{
	class DirectCompositionWindow : public Window
	{
		friend void PGUI::Initialize();
		
		public:
		explicit DirectCompositionWindow(const WindowClass::WindowClassPtr& wndClass) noexcept;

		protected:
		[[nodiscard]] static ComPtr<ID3D11Device2> D3D11Device() noexcept;
		[[nodiscard]] static ComPtr<IDXGIDevice4> DXGIDevice() noexcept;
		[[nodiscard]] static ComPtr<IDCompositionDevice> DCompositionDevice() noexcept;
		[[nodiscard]] static ComPtr<ID2D1Device7> D2D1Device() noexcept;
		[[nodiscard]] ComPtr<IDXGISwapChain1> DXGISwapChain() const noexcept;
		[[nodiscard]] ComPtr<IDCompositionTarget> DCompositionTarget() const noexcept;
		[[nodiscard]] ComPtr<ID2D1DeviceContext7> D2D1DeviceContext() const noexcept;

		[[nodiscard]] Graphics::Graphics GetGraphics() const noexcept;

		virtual void BeginDraw();
		virtual HRESULT EndDraw();

		virtual void CreateDeviceResources();
		virtual void DiscardDeviceResources();

		private:
		inline static ComPtr<ID3D11Device2> d3d11Device;
		inline static ComPtr<IDXGIDevice4> dxgiDevice;
		inline static ComPtr<IDCompositionDevice> dcompDevice;
		inline static ComPtr<ID2D1Device7> d2d1Device;
		ComPtr<IDXGISwapChain1> swapChain;
		ComPtr<IDCompositionTarget> dcompTarget;
		ComPtr<ID2D1DeviceContext7> d2d1Dc;

		static void InitD3D11Device();
		static void InitDCompDevice();
		static void InitD2D1Device();
		void InitSwapChain();
		void InitD2D1DeviceContext();
		void InitDirectComposition();

		Core::HandlerResult OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam);
		Core::HandlerResult OnSize(UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
