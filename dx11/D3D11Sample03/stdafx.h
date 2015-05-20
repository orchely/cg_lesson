#pragma once

#define WIN32_LEAN_AND_MEAN
#define _ATL_NO_AUTOMATIC_NAMESPACE
#if defined(_DEBUG)
#define _ATL_DEBUG_INTERFACES
#endif

#include <sstream>
#include <ostream>
#include <iomanip>
#include <array>

#include "targetver.h"
#include <windows.h>
#include <crtdbg.h>
#include <atlbase.h>

#include <d3d11.h>