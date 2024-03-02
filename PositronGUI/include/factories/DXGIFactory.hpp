#pragma once

#include "core/Exceptions.hpp"
#include "helpers/ComPtr.hpp"

#include <dxgi1_3.h>
#include <Windows.h>


namespace PGUI
{
	class DXGIFactory
	{
		public:
		DXGIFactory() = delete;
		DXGIFactory(DXGIFactory&) = delete;
		void operator=(const DXGIFactory&) = delete;

		static [[nodiscard]] ComPtr<IDXGIFactory2> GetFactory()
		{
			if (!dxgiFactory)
			{
				UINT flags = 0;
				
				#ifdef _DEBUG
				flags = DXGI_CREATE_FACTORY_DEBUG;
				#endif

				HRESULT hr = CreateDXGIFactory2(flags, __uuidof(IDXGIFactory2), (void**)(dxgiFactory.GetAddressOf()));
				if (FAILED(hr))
				{
					throw Core::ErrorHandling::HresultException{ hr };
				}
			}
			return dxgiFactory;
		}

		private:
		static inline ComPtr<IDXGIFactory2> dxgiFactory = nullptr;
	};
}
