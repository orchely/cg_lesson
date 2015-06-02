#include "stdafx.h"
#include "D3D11SampleCommon.h"

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

HRESULT readFile(const wchar_t *filename, std::vector<uint8_t> &buf)
{
	HANDLE file = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		HRESULT error = HRESULT_FROM_WIN32(GetLastError());
		std::wostringstream oss;
		oss << L"CreateFile(\"" << filename << L"\", ...) failed.";
		return TRACE_ERROR(error, oss.str().c_str());
	}
	auto closer = finally([=]{ CloseHandle(file); });

	LARGE_INTEGER size;
	if (FAILED(GetFileSizeEx(file, &size))) {
		return TRACE_ERROR(HRESULT_FROM_WIN32(GetLastError()), L"GetFileSizeEx() failed.");
	}
	buf.resize(size.QuadPart);

	DWORD readSize;
	if (ReadFile(file, buf.data(), static_cast<DWORD>(buf.size()), &readSize, nullptr) == 0) {
		return TRACE_ERROR(HRESULT_FROM_WIN32(GetLastError()), L"ReadFile() failed.");
	}

	return S_OK;
}