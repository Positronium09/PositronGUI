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
		DXGIFactory(DXGIFactory&) = delete;
		void operator=(const DXGIFactory&) = delete;

		[[nodiscard]] static ComPtr<IDXGIFactory7> GetFactory()
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
