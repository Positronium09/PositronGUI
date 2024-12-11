#pragma once

#include "helpers/HelperFunctions.hpp"
#include "helpers/ComPtr.hpp"

#include <dwrite_3.h>
#include <Windows.h>


namespace PGUI
{
	class DWriteFactory
	{
		public:
		DWriteFactory() = delete;
		DWriteFactory(const DWriteFactory&) = delete;
		auto operator=(const DWriteFactory&) -> DWriteFactory& = delete;
		DWriteFactory(DWriteFactory&&) = delete;
		auto operator=(DWriteFactory&&) -> DWriteFactory& = delete;
		~DWriteFactory() = default;

		[[nodiscard]] static auto GetFactory()
		{
			if (!directWriteFactory)
			{
				HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, 
					__uuidof(IDWriteFactory8), (IUnknown**)directWriteFactory.GetAddressOf()); HR_T(hr);
			}
			return directWriteFactory;
		}

		private:
		static inline ComPtr<IDWriteFactory8> directWriteFactory = nullptr;
	};
}
