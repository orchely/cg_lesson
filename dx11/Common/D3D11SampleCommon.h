#pragma once

#include "stdafx.h"

#if defined(_DEBUG)

#define TRACE_ERROR(hr, message) TraceError(__FILEW__, __LINE__, hr, message)
HRESULT TraceError(const wchar_t *file, int line, HRESULT hr, const wchar_t *message);

#else

#define TRACE_ERROR(hr, message) (hr)

#endif

HRESULT readFile(const wchar_t *filename, std::vector<uint8_t> &buf);

// From The C++ Programing Language 4th Edition, chapter 13

template<typename F>
struct Final_action
{
	Final_action(F f) : clean(f) {}
	~Final_action() { clean(); }
	F clean;
};

template<typename F>
Final_action<F> finally(F f)
{
	return Final_action<F>(f);
}