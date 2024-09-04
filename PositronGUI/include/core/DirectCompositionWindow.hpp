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
		[[nodiscard]] static auto D3D11Device() noexcept -> ComPtr<ID3D11Device2>;
		[[nodiscard]] static auto DXGIDevice() noexcept -> ComPtr<IDXGIDevice4>;
		[[nodiscard]] static auto DCompositionDevice() noexcept -> ComPtr<IDCompositionDevice>;
		[[nodiscard]] static auto D2D1Device() noexcept -> ComPtr<ID2D1Device7>;
		[[nodiscard]] auto DXGISwapChain() const noexcept -> ComPtr<IDXGISwapChain1>;
		[[nodiscard]] auto DCompositionTarget() const noexcept -> ComPtr<IDCompositionTarget>;
		[[nodiscard]] auto D2D1DeviceContext() const noexcept -> ComPtr<ID2D1DeviceContext7>;

		[[nodiscard]] auto GetGraphics() const noexcept -> Graphics::Graphics;

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
