#pragma once


#include <wrl/client.h>

namespace PGUI
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}