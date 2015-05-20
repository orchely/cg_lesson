#include "stdafx.h"
#include "finally.h"

HRESULT TraceError(const wchar_t *file, int line, HRESULT hr, const wchar_t *message)
{
	std::wostringstream oss;
	oss << file << L"(" << line << L"): ";
	oss << L"0x" << std::hex << std::nouppercase << std::setfill(L'0') << std::setw(8) << hr;

	DWORD flag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;
	LPWSTR buf = nullptr; 
	FormatMessageW(flag, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&buf), 0, nullptr);
	auto fin = finally([=]{ if (buf != nullptr) LocalFree(buf); });

	if (buf != nullptr) {
		std::wstring string{ buf };
		// trim tail spaces
		size_t endpos = string.find_last_not_of(L" ");
		if (endpos != std::wstring::npos) {
			string = string.substr(0, endpos + 1);
		}
		oss << L"(" << string << L")";
	}

	if (message != nullptr) {
		oss << L", " << message;
	}

	oss << std::endl;

	OutputDebugStringW(oss.str().c_str());

	return hr;
}