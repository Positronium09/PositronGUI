#pragma once

#include "helpers/HelperFunctions.hpp"
#include "helpers/ComPtr.hpp"

#include <dxgi1_6.h>
#include <Windows.h>


namespace PGUI
{
	class DXGIFactory
	{
		public:
		DXGIFactory() = delete;
		DXGIFactory(const DXGIFactory&) = delete;
		auto operator=(const DXGIFactory&) -> DXGIFactory& = delete;
		DXGIFactory(DXGIFactory&&) = delete;
		auto operator=(DXGIFactory&&) -> DXGIFactory& = delete;
		~DXGIFactory() = default;

		[[nodiscard]] static auto GetFactory()
		{
			if (!dxgiFactory)
			{
				UINT flags = 0;
				
				#ifdef _DEBUG
				flags = DXGI_CREATE_FACTORY_DEBUG;
				#endif

				HRESULT hr = CreateDXGIFactory2(flags, 
					__uuidof(IDXGIFactory7), (void**)(dxgiFactory.GetAddressOf())); HR_T(hr);
			}
			return dxgiFactory;
		}

		private:
		static inline ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	};
}
