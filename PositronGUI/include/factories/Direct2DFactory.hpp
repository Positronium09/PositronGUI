#pragma once

#include "core/Exceptions.hpp"
#include "helpers/ComPtr.hpp"

#include <d2d1_2.h>
#include <d2d1.h>
#include <Windows.h>


namespace PGUI
{
	class Direct2DFactory
	{
		public:
		Direct2DFactory() = delete;
		Direct2DFactory(Direct2DFactory&) = delete;
		void operator=(const Direct2DFactory&) = delete;

		static ComPtr<ID2D1Factory2> GetFactory()
		{
			if (!direct2DFactory)
			{
				D2D1_FACTORY_OPTIONS options{ };

				#ifdef _DEBUG
				options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
				#endif

				HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, direct2DFactory.GetAddressOf());
				if (FAILED(hr))
				{
					throw Core::ErrorHandling::HresultException{ hr };
				}
			}
			return direct2DFactory;
		}

		private:
		static inline ComPtr<ID2D1Factory2> direct2DFactory = nullptr;
	};
}
