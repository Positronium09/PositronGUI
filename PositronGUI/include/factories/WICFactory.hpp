#pragma once

#include "core/Exceptions.hpp"
#include "helpers/ComPtr.hpp"

#include <bit>
#include <wincodec.h>


namespace PGUI
{
	class WICFactory
	{
		public:
		WICFactory() = delete;
		WICFactory(WICFactory&) = delete;
		void operator=(const WICFactory&) = delete;

		static [[nodiscard]] ComPtr<IWICImagingFactory> GetFactory()
		{
			if (!wicFactory)
			{
				HRESULT hr = CoCreateInstance(
					CLSID_WICImagingFactory,
					nullptr,
					CLSCTX_INPROC_SERVER,
					__uuidof(IWICImagingFactory), std::bit_cast<void**>(wicFactory.GetAddressOf()));
				if (FAILED(hr))
				{
					throw Core::ErrorHandling::HresultException{ hr };
				}
			}
			return wicFactory;
		}

		private:
		static inline ComPtr<IWICImagingFactory> wicFactory = nullptr;
	};
}
