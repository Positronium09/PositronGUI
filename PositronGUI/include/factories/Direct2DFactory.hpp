#pragma once

#include "helpers/HelperFunctions.hpp"
#include "helpers/ComPtr.hpp"

#include <d2d1_3.h>
#include <d2d1.h>
#include <Windows.h>


namespace PGUI
{
	class D2DFactory
	{
		public:
		D2DFactory() = delete;
		D2DFactory(D2DFactory&) = delete;
		void operator=(const D2DFactory&) = delete;

		static [[nodiscard]] ComPtr<ID2D1Factory8> GetFactory()
		{
			if (!direct2DFactory)
			{
				D2D1_FACTORY_OPTIONS options{ };

				#ifdef _DEBUG
				options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
				#endif

				HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, 
					options, direct2DFactory.GetAddressOf()); HR_T(hr);
			}
			return direct2DFactory;
		}

		private:
		static inline ComPtr<ID2D1Factory8> direct2DFactory = nullptr;
	};
}
