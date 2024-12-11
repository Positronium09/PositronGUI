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

		[[nodiscard]] auto GetGraphics() const noexcept { return Graphics::Graphics{ d2d1Dc }; }

		protected:
		[[nodiscard]] static auto D3D11Device() noexcept { return d3d11Device; }
		[[nodiscard]] static auto DXGIDevice() noexcept { return dxgiDevice; }
		[[nodiscard]] static auto DCompositionDevice() noexcept { return dcompDevice; }
		[[nodiscard]] static auto D2D1Device() noexcept { return d2d1Device; }
		[[nodiscard]] auto DXGISwapChain() const noexcept { return swapChain; }
		[[nodiscard]] auto DCompositionTarget() const noexcept { return dcompTarget; }
		[[nodiscard]] auto D2D1DeviceContext() const noexcept { return d2d1Dc; }

		virtual void BeginDraw();
		virtual auto EndDraw() -> HRESULT;

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

		auto OnNCCreate(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
		auto OnSize(UINT msg, WPARAM wParam, LPARAM lParam) -> Core::HandlerResult;
	};
}
