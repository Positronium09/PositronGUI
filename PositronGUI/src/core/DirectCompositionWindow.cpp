#include "core/DirectCompositionWindow.hpp"

#include "core/Exceptions.hpp"
#include "helpers/HelperFunctions.hpp"
#include "factories/DXGIFactory.hpp"
#include "factories/Direct2DFactory.hpp"
 
#include <array>


namespace PGUI::Core
{
	DirectCompositionWindow::DirectCompositionWindow(const WindowClass::WindowClassPtr& wndClass) noexcept :
		Window{ wndClass }
	{
		RegisterMessageHandler(WM_NCCREATE, &DirectCompositionWindow::OnNCCreate);
		RegisterMessageHandler(WM_SIZE, &DirectCompositionWindow::OnSize);
	}

	void DirectCompositionWindow::BeginDraw()
	{
		CreateDeviceResources();

		d2d1Dc->BeginDraw();
	}

	auto DirectCompositionWindow::EndDraw() -> HRESULT
	{
		HRESULT hr = d2d1Dc->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			DiscardDeviceResources();
		}
		HR_L(hr);

		hr = swapChain->Present(1, NULL); HR_L(hr);
		return hr;
	}

	void DirectCompositionWindow::CreateDeviceResources()
	{
		/* Not pure virtual to be optional to override */
	}

	void DirectCompositionWindow::DiscardDeviceResources()
	{
		/* Not pure virtual to be optional to override */
	}

	void DirectCompositionWindow::InitD3D11Device()
	{
		if (d3d11Device)
		{
			return;
		}

		auto dxgiFactory = DXGIFactory::GetFactory();
		SYSTEM_POWER_STATUS powerStatus{ };
		GetSystemPowerStatus(&powerStatus);

		DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		if (powerStatus.SystemStatusFlag == 1 ||
			(powerStatus.BatteryFlag & (2 | 4) && ~(powerStatus.BatteryFlag & 8)))
		{
			gpuPreference = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
		}

		ComPtr<IDXGIAdapter1> adapter;
		HRESULT hr = dxgiFactory->EnumAdapterByGpuPreference(0, gpuPreference,
			IID_PPV_ARGS(adapter.GetAddressOf())); HR_T(hr);

		auto createDeviceFlags =
			#ifdef _DEBUG
		(D3D11_CREATE_DEVICE_FLAG)(D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG);
		#else
			D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#endif

		std::array featureLevels =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		ComPtr<ID3D11Device> device;
		hr = D3D11CreateDevice(adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN, nullptr,
			createDeviceFlags,
			featureLevels.data(),
			static_cast<UINT>(featureLevels.size()),
			D3D11_SDK_VERSION,
			&device, nullptr, nullptr);  HR_T(hr);
		hr = device.As(&d3d11Device); HR_T(hr);
		hr = d3d11Device.As(&dxgiDevice); HR_T(hr);
}

	void DirectCompositionWindow::InitDCompDevice()
	{
		HRESULT hr = DCompositionCreateDevice(
			dxgiDevice.Get(),
			__uuidof(dcompDevice),
			std::bit_cast<void**>(&dcompDevice)); HR_T(hr);
	}

	void DirectCompositionWindow::InitSwapChain()
	{
		auto dxgiFactory = DXGIFactory::GetFactory();

		DXGI_SWAP_CHAIN_DESC1 description = {};
		description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		description.BufferCount = 2;
		description.SampleDesc.Count = 1;
		description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		description.Width = 1;
		description.Height = 1;

		HRESULT hr = dxgiFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
			&description, nullptr,
			&swapChain); HR_T(hr);
	}

	void DirectCompositionWindow::InitD2D1Device()
	{
		auto d2Factory = D2DFactory::GetFactory();

		HRESULT hr = d2Factory->CreateDevice(dxgiDevice.Get(),
			&d2d1Device); HR_T(hr);
	}

	void DirectCompositionWindow::InitD2D1DeviceContext()
	{
		HRESULT hr = d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&d2d1Dc); HR_T(hr);

		ComPtr<IDXGISurface2> surface;
		hr = swapChain->GetBuffer(0, IID_PPV_ARGS(surface.GetAddressOf())); HR_T(hr);

		D2D1_BITMAP_PROPERTIES1 properties = {};
		properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

		ComPtr<ID2D1Bitmap1> bitmap;
		hr = d2d1Dc->CreateBitmapFromDxgiSurface(surface.Get(),
			properties,
			&bitmap); HR_T(hr);

		d2d1Dc->SetTarget(bitmap.Get());
	}

	void DirectCompositionWindow::InitDirectComposition()
	{
		HRESULT hr = dcompDevice->CreateTargetForHwnd(Hwnd(), false,
			&dcompTarget); HR_T(hr);

		ComPtr<IDCompositionVisual> visual;
		hr = dcompDevice->CreateVisual(&visual); HR_T(hr);

		hr = visual->SetContent(swapChain.Get()); HR_T(hr);
		hr = dcompTarget->SetRoot(visual.Get()); HR_T(hr);

		hr = dcompDevice->Commit(); HR_T(hr);
	}

	auto DirectCompositionWindow::OnNCCreate(UINT, WPARAM, LPARAM) -> Core::HandlerResult
	{
		InitSwapChain();
		InitD2D1DeviceContext();
		InitDirectComposition();

		return { 1, HandlerResultFlag::PassToDefWindowProc };
	}

	auto DirectCompositionWindow::OnSize(UINT, WPARAM, LPARAM) -> Core::HandlerResult
	{
		auto size = GetWindowSize();

		d2d1Dc->SetTarget(nullptr);

		if (size.cy == 0)
		{
			size.cy = 1;
		}

		HRESULT hr = swapChain->ResizeBuffers(0, size.cx, size.cy,
			DXGI_FORMAT_UNKNOWN, NULL); HR_T(hr);

		DiscardDeviceResources();
		InitD2D1DeviceContext();

		return 0;
	}

	auto DirectCompositionWindow::D3D11Device() noexcept -> ComPtr<ID3D11Device2>
	{
		return d3d11Device;
	}
	auto DirectCompositionWindow::DXGIDevice() noexcept -> ComPtr<IDXGIDevice4>
	{
		return dxgiDevice;
	}
	auto DirectCompositionWindow::DXGISwapChain() const noexcept -> ComPtr<IDXGISwapChain1>
	{
		return swapChain;
	}
	auto DirectCompositionWindow::DCompositionDevice() noexcept -> ComPtr<IDCompositionDevice>
	{
		return dcompDevice;
	}
	auto DirectCompositionWindow::DCompositionTarget() const noexcept -> ComPtr<IDCompositionTarget>
	{
		return dcompTarget;
	}
	auto DirectCompositionWindow::D2D1Device() noexcept -> ComPtr<ID2D1Device7>
	{
		return d2d1Device;
	}
	auto DirectCompositionWindow::D2D1DeviceContext() const noexcept -> ComPtr<ID2D1DeviceContext7>
	{
		return d2d1Dc;
	}

	auto DirectCompositionWindow::GetGraphics() const noexcept -> Graphics::Graphics
	{
		return Graphics::Graphics{ D2D1DeviceContext() };
	}
}
