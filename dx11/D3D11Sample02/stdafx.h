#pragma once

#define WIN32_LEAN_AND_MEAN
#define _ATL_NO_AUTOMATIC_NAMESPACE
#if defined(_DEBUG)
#define _ATL_DEBUG_INTERFACES
#endif

#include <stdio.h>

#include <sstream>
#include <ostream>
#include <iomanip>
#include <array>
#include <vector>

#include "targetver.h"
#include <Windows.h>
#include <crtdbg.h>
#include <atlbase.h>

#include <dxgi.h>
#include <d3d11.h>