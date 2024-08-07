#pragma once

#include <d2d1.h>


namespace PGUI::Graphics
{
	enum class AntialiasMode
	{
		PerPrimitive = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		Aliased = D2D1_ANTIALIAS_MODE_ALIASED
	};
}
