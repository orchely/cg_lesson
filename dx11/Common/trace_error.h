#pragma once

#include "stdafx.h"

#if defined(_DEBUG)

#define TRACE_ERROR(hr, message) TraceError(__FILEW__, __LINE__, hr, message)
HRESULT TraceError(const wchar_t *file, int line, HRESULT hr, const wchar_t *message);

#else

#define TRACE_ERROR(hr, message) (hr)

#endif