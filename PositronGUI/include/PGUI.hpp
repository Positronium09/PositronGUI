#pragma once

#include "core/PGUI.core.hpp"
#include "helpers/PGUI.helpers.hpp"
#include "ui/PGUI.ui.hpp"
#include <winrt/base.h>


namespace PGUI
{
	void Initialize();
	void InitializeWindowsFoundation();
	void UninitializeWindowsFoundation();
}