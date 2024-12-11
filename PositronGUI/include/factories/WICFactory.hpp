#pragma once

#include "helpers/HelperFunctions.hpp"
#include "helpers/ComPtr.hpp"

#include <bit>
#include <wincodec.h>


namespace PGUI
{
	class WICFactory
	{
		public:
		WICFactory() = delete;
		WICFactory(const WICFactory&) = delete;
		auto operator=(const WICFactory&) -> WICFactory& = delete;
		WICFactory(WICFactory&&) = delete;
		auto operator=(WICFactory&&) -> WICFactory& = delete;
		~WICFactory() = default;

		[[nodiscard]] static auto GetFactory()
		{
			if (!wicFactory)
			{
				HRESULT hr = CoCreateInstance(
					CLSID_WICImagingFactory,
					nullptr,
					CLSCTX_INPROC_SERVER,
					__uuidof(IWICImagingFactory), std::bit_cast<void**>(wicFactory.GetAddressOf())); HR_T(hr);
			}
			return wicFactory;
		}

		private:
		static inline ComPtr<IWICImagingFactory> wicFactory = nullptr;
	};
}
