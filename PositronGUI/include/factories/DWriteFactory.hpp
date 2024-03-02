#pragma once

#include "core/Exceptions.hpp"
#include "helpers/ComPtr.hpp"

#include <dwrite_3.h>
#include <Windows.h>


namespace PGUI
{
	class DirectWriteFactory
	{
		public:
		DirectWriteFactory() = delete;
		DirectWriteFactory(DirectWriteFactory&) = delete;
		void operator=(const DirectWriteFactory&) = delete;

		static [[nodiscard]] ComPtr<IDWriteFactory3> GetFactory()
		{
			if (!directWriteFactory)
			{
				HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), (IUnknown**)directWriteFactory.GetAddressOf());
				if (FAILED(hr))
				{
					throw Core::ErrorHandling::HresultException{ hr };
				}
			}
			return directWriteFactory;
		}

		private:
		static inline ComPtr<IDWriteFactory3> directWriteFactory = nullptr;
	};
}
