#pragma once

#include "core/PGUI.core.hpp"
#include "graphics/PGUI.graphics.hpp"
#include "helpers/PGUI.helpers.hpp"
#include "ui/PGUI.ui.hpp"
#include "graphics/PGUI.graphics.hpp"
#include <winrt/base.h>


namespace PGUI
{
	void Initialize();
	void Uninitialize();
	void InitializeWindowsFoundation();
	void UninitializeWindowsFoundation();
}
