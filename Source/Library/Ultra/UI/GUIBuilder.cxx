﻿module;

// Hack: The included header unit in the Core module doesn't work here.
#include "Ultra/Core/Core.h"

// Properties
#define IMGUI_IMPL_OPENGL_LOADER_GLAD2

// Hack: Workaround for problems with 'Windows.h' in combination with C++ modules (VS2022 > v17.5)!
#undef __nullnullterminated
#define __SPECSTRINGS_STRICT_LEVEL 0

// Graphics Backends
#include <backends/imgui_impl_opengl3.cpp>
#include <backends/imgui_impl_vulkan.cpp>

// System Backends
#ifdef APP_PLATFORM_WINDOWS
    #include <backends/imgui_impl_win32-patch.cpp>
#endif

// Hack: Workaround for problems with 'Windows.h' in combination with C++ modules (VS2022 > v17.5)!
#undef __nullnullterminated
#define __SPECSTRINGS_STRICT_LEVEL 0

module Ultra.UI.GUIBuilder;