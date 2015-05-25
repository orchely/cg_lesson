#include "stdafx.h"
#include <D3D11SampleCommon.h>
#include <DirectXMath.h>

WCHAR* FormatName[] = {
	L"DXGI_FORMAT_UNKNOWN",
	L"DXGI_FORMAT_R32G32B32A32_TYPELESS",
	L"DXGI_FORMAT_R32G32B32A32_FLOAT",
	L"DXGI_FORMAT_R32G32B32A32_UINT",
	L"DXGI_FORMAT_R32G32B32A32_SINT",
	L"DXGI_FORMAT_R32G32B32_TYPELESS",
	L"DXGI_FORMAT_R32G32B32_FLOAT",
	L"DXGI_FORMAT_R32G32B32_UINT",
	L"DXGI_FORMAT_R32G32B32_SINT",
	L"DXGI_FORMAT_R16G16B16A16_TYPELESS",
	L"DXGI_FORMAT_R16G16B16A16_FLOAT",
	L"DXGI_FORMAT_R16G16B16A16_UNORM",
	L"DXGI_FORMAT_R16G16B16A16_UINT",
	L"DXGI_FORMAT_R16G16B16A16_SNORM",
	L"DXGI_FORMAT_R16G16B16A16_SINT",
	L"DXGI_FORMAT_R32G32_TYPELESS",
	L"DXGI_FORMAT_R32G32_FLOAT",
	L"DXGI_FORMAT_R32G32_UINT",
	L"DXGI_FORMAT_R32G32_SINT",
	L"DXGI_FORMAT_R32G8X24_TYPELESS",
	L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
	L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
	L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
	L"DXGI_FORMAT_R10G10B10A2_TYPELESS",
	L"DXGI_FORMAT_R10G10B10A2_UNORM",
	L"DXGI_FORMAT_R10G10B10A2_UINT",
	L"DXGI_FORMAT_R11G11B10_FLOAT",
	L"DXGI_FORMAT_R8G8B8A8_TYPELESS",
	L"DXGI_FORMAT_R8G8B8A8_UNORM",
	L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
	L"DXGI_FORMAT_R8G8B8A8_UINT",
	L"DXGI_FORMAT_R8G8B8A8_SNORM",
	L"DXGI_FORMAT_R8G8B8A8_SINT",
	L"DXGI_FORMAT_R16G16_TYPELESS",
	L"DXGI_FORMAT_R16G16_FLOAT",
	L"DXGI_FORMAT_R16G16_UNORM",
	L"DXGI_FORMAT_R16G16_UINT",
	L"DXGI_FORMAT_R16G16_SNORM",
	L"DXGI_FORMAT_R16G16_SINT",
	L"DXGI_FORMAT_R32_TYPELESS",
	L"DXGI_FORMAT_D32_FLOAT",
	L"DXGI_FORMAT_R32_FLOAT",
	L"DXGI_FORMAT_R32_UINT",
	L"DXGI_FORMAT_R32_SINT",
	L"DXGI_FORMAT_R24G8_TYPELESS",
	L"DXGI_FORMAT_D24_UNORM_S8_UINT",
	L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
	L"DXGI_FORMAT_X24_TYPELESS_G8_UINT",
	L"DXGI_FORMAT_R8G8_TYPELESS",
	L"DXGI_FORMAT_R8G8_UNORM",
	L"DXGI_FORMAT_R8G8_UINT",
	L"DXGI_FORMAT_R8G8_SNORM",
	L"DXGI_FORMAT_R8G8_SINT",
	L"DXGI_FORMAT_R16_TYPELESS",
	L"DXGI_FORMAT_R16_FLOAT",
	L"DXGI_FORMAT_D16_UNORM",
	L"DXGI_FORMAT_R16_UNORM",
	L"DXGI_FORMAT_R16_UINT",
	L"DXGI_FORMAT_R16_SNORM",
	L"DXGI_FORMAT_R16_SINT",
	L"DXGI_FORMAT_R8_TYPELESS",
	L"DXGI_FORMAT_R8_UNORM",
	L"DXGI_FORMAT_R8_UINT",
	L"DXGI_FORMAT_R8_SNORM",
	L"DXGI_FORMAT_R8_SINT",
	L"DXGI_FORMAT_A8_UNORM",
	L"DXGI_FORMAT_R1_UNORM",
	L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
	L"DXGI_FORMAT_R8G8_B8G8_UNORM",
	L"DXGI_FORMAT_G8R8_G8B8_UNORM",
	L"DXGI_FORMAT_BC1_TYPELESS",
	L"DXGI_FORMAT_BC1_UNORM",
	L"DXGI_FORMAT_BC1_UNORM_SRGB",
	L"DXGI_FORMAT_BC2_TYPELESS",
	L"DXGI_FORMAT_BC2_UNORM",
	L"DXGI_FORMAT_BC2_UNORM_SRGB",
	L"DXGI_FORMAT_BC3_TYPELESS",
	L"DXGI_FORMAT_BC3_UNORM",
	L"DXGI_FORMAT_BC3_UNORM_SRGB",
	L"DXGI_FORMAT_BC4_TYPELESS",
	L"DXGI_FORMAT_BC4_UNORM",
	L"DXGI_FORMAT_BC4_SNORM",
	L"DXGI_FORMAT_BC5_TYPELESS",
	L"DXGI_FORMAT_BC5_UNORM",
	L"DXGI_FORMAT_BC5_SNORM",
	L"DXGI_FORMAT_B5G6R5_UNORM",
	L"DXGI_FORMAT_B5G5R5A1_UNORM",
	L"DXGI_FORMAT_B8G8R8A8_UNORM",
	L"DXGI_FORMAT_B8G8R8X8_UNORM",
	L"DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",
	L"DXGI_FORMAT_B8G8R8A8_TYPELESS",
	L"DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
	L"DXGI_FORMAT_B8G8R8X8_TYPELESS",
	L"DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
	L"DXGI_FORMAT_BC6H_TYPELESS",
	L"DXGI_FORMAT_BC6H_UF16",
	L"DXGI_FORMAT_BC6H_SF16",
	L"DXGI_FORMAT_BC7_TYPELESS",
	L"DXGI_FORMAT_BC7_UNORM",
	L"DXGI_FORMAT_BC7_UNORM_SRGB",
	L"DXGI_FORMAT_AYUV",
	L"DXGI_FORMAT_Y410",
	L"DXGI_FORMAT_Y416",
	L"DXGI_FORMAT_NV12",
	L"DXGI_FORMAT_P010",
	L"DXGI_FORMAT_P016",
	L"DXGI_FORMAT_420_OPAQUE",
	L"DXGI_FORMAT_YUY2",
	L"DXGI_FORMAT_Y210",
	L"DXGI_FORMAT_Y216",
	L"DXGI_FORMAT_NV11",
	L"DXGI_FORMAT_AI44",
	L"DXGI_FORMAT_IA44",
	L"DXGI_FORMAT_P8",
	L"DXGI_FORMAT_A8P8",
	L"DXGI_FORMAT_B4G4R4A4_UNORM"
};

WCHAR* RotationName[] = {
	L"DXGI_MODE_ROTATION_UNSPECIFIED",
	L"DXGI_MODE_ROTATION_IDENTITY",
	L"DXGI_MODE_ROTATION_ROTATE90",
	L"DXGI_MODE_ROTATION_ROTATE180",
	L"DXGI_MODE_ROTATION_ROTATE270"
};

WCHAR* ScanlineName[] = {
	L"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
	L"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
	L"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
	L"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
};

WCHAR* ScalingName[] = {
	L"DXGI_MODE_SCALING_UNSPECIFIED",
	L"DXGI_MODE_SCALING_CENTERED",
	L"DXGI_MODE_SCALING_STRETCHED"
};

WCHAR* FeatureLevelName[] = {
	L"D3D_FEATURE_LEVEL_9_1",
	L"D3D_FEATURE_LEVEL_9_2",
	L"D3D_FEATURE_LEVEL_9_3",
	L"D3D_FEATURE_LEVEL_10_0",
	L"D3D_FEATURE_LEVEL_10_1",
	L"D3D_FEATURE_LEVEL_11_0",
	L"D3D_FEATURE_LEVEL_11_1"
};

UINT FeatureLevelNumber[] = {
	0x9100,
	0x9200,
	0x9300,
	0xa000,
	0xa100,
	0xb000,
	0xb100
};

int wmain(int argc, wchar_t* argv[])
{
	ATL::CComPtr<IDXGIFactory> factory;
	{
		IDXGIFactory *p = nullptr;
		if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&p)))) {
			fwprintf_s(stderr, L"CreateDXGIFactory() failed.\n");
			return 0;
		}
		factory.Attach(p);
	}

	UINT adapterIndex = 0;
	while (true) {
		ATL::CComPtr<IDXGIAdapter> adapter;
		{
			IDXGIAdapter *p = nullptr;
			HRESULT result = factory->EnumAdapters(adapterIndex, &p);
			if (result == DXGI_ERROR_NOT_FOUND) {
				break;
			} else if (FAILED(result)) {
				fwprintf_s(stderr, L"IDXGIFactory::EnumAdapters() failed\n");
				return 0;
			}
			adapter.Attach(p);
		}

		wprintf_s(L"-- adapter(%u) --\n", adapterIndex);
		
		DXGI_ADAPTER_DESC adapterDesc;
		if (FAILED(adapter->GetDesc(&adapterDesc))) {
			fwprintf_s(stderr, L"IDXGIAdapter::GetDesc() failed.\n");
			return 0;
		}

		wprintf_s(L"Description : %s\n", adapterDesc.Description);
		wprintf_s(L"VendorId    : 0x%x\n", adapterDesc.VendorId);
		wprintf_s(L"DeviceId    : 0x%x\n", adapterDesc.DeviceId);
		wprintf_s(L"SubSysId    : %u\n", adapterDesc.SubSysId);
		wprintf_s(L"Revision    : %u\n", adapterDesc.Revision);
		wprintf_s(L"DedicatedVideoMemory  : %lu\n", adapterDesc.DedicatedVideoMemory);
		wprintf_s(L"DedicatedSystemMemory : %lu\n", adapterDesc.DedicatedSystemMemory);
		wprintf_s(L"SharedSystemMemory    : %lu\n", adapterDesc.SharedSystemMemory);

		UINT outputIndex = 0;
		while (true) {
			ATL::CComPtr<IDXGIOutput> output;
			{
				IDXGIOutput *p = nullptr;
				HRESULT result = adapter->EnumOutputs(outputIndex, &p);
				if (result == DXGI_ERROR_NOT_FOUND) {
					break;
				} else if (FAILED(result)) {
					fwprintf_s(stderr, L"IDXGIAdapter::EnumOutputs() failed.\n");
					return 0;
				}
				output.Attach(p);
			}

			wprintf_s(L"  -- output(%u) --\n", outputIndex);

			DXGI_OUTPUT_DESC outputDesc;
			if (FAILED(output->GetDesc(&outputDesc))) {
				fwprintf_s(stderr, L"IDXGIOutput::GetDesc() failed.\n");
			}

			const RECT &rect = outputDesc.DesktopCoordinates;
			wprintf_s(L"  DeviceName          : %s\n", outputDesc.DeviceName);
			wprintf_s(L"  DescktopCoordinates : (%d, %d, %d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
			wprintf_s(L"  AttachedToDesktop   : %s\n", outputDesc.AttachedToDesktop ? L"TRUE" : L"FALSE");
			wprintf_s(L"  Rotation            : %s\n", RotationName[outputDesc.Rotation]);

			for (size_t formatIndex = 0; formatIndex < sizeof(FormatName) / sizeof(FormatName[0]); formatIndex++) {
				DXGI_FORMAT format = static_cast<DXGI_FORMAT>(formatIndex);

				UINT modes = 0;
				if (FAILED(output->GetDisplayModeList(format, 0, &modes, 0))) {
					fwprintf_s(stderr, L"IDXGIOutput::GetDisplayModeList() failed.\n");
					return 0;
				}
				if (modes == 0) {
					continue;
				}

				wprintf_s(L"  [%s : %u]\n", FormatName[formatIndex], modes);

				std::vector<DXGI_MODE_DESC> modeDesc(modes);
				if (FAILED(output->GetDisplayModeList(format, 0, &modes, modeDesc.data()))) {
					fwprintf_s(stderr, L"IDXGIOutput::GetDisplayModeList() failed.\n");
					return 0;
				}

				for (size_t i = 0; i < modeDesc.size(); i++) {
					wprintf_s(L"    -- DisplayMode(%llu) --\n", i);
					wprintf_s(L"    Width            : %u\n", modeDesc[i].Width);
					wprintf_s(L"    Height           : %u\n", modeDesc[i].Height);
					wprintf_s(L"    RefreshRate      : %u/%u\n", modeDesc[i].RefreshRate.Numerator, modeDesc[i].RefreshRate.Denominator);
					wprintf_s(L"    Format           : %s\n", FormatName[modeDesc[i].Format]);
					wprintf_s(L"    ScanlineOrdering : %s\n", ScanlineName[modeDesc[i].ScanlineOrdering]);
					wprintf_s(L"    Scaling          : %s\n", ScalingName[modeDesc[i].Scaling]);
				}
			}
			outputIndex++;
		}
		adapterIndex++;
	}

	HRESULT result = S_OK;
	ATL::CComPtr<ID3D11Device> device;
	ATL::CComPtr<ID3D11DeviceContext> deviceContext;
	D3D_FEATURE_LEVEL featureLevel;

	for (auto driverType : { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE }) {
		ID3D11Device *d = nullptr;
		ID3D11DeviceContext *dc = nullptr;
		result = D3D11CreateDevice(nullptr, driverType, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &d, &featureLevel, &dc);
		if (SUCCEEDED(result)) {
			switch (driverType) {
			case D3D_DRIVER_TYPE_HARDWARE:	wprintf_s(L"[D3D_DRIVER_TYPE_HARDWARE]\n");		break;
			case D3D_DRIVER_TYPE_WARP:		wprintf_s(L"[D3D_DRIVER_TYPE_WARP]\n");			break;
			case D3D_DRIVER_TYPE_REFERENCE:	wprintf_s(L"[D3D_DRIVER_TYPE_REFERENCE]\n");	break;
			}
			device.Attach(d);
			deviceContext.Attach(dc);
			break;
		}
	}

	if (FAILED(result)) {
		fwprintf_s(stderr, L"D3D11CreateDevice() failed.\n");
		return 0;
	}

	for (size_t i = 0; i < sizeof(FeatureLevelNumber) / sizeof(FeatureLevelNumber[0]); i++) {
		if (FeatureLevelNumber[i] == featureLevel) {
			wprintf_s(L"[%s]\n", FeatureLevelName[i]);
			break;
		}
	}

	for (size_t i = 0; i < sizeof(FormatName) / sizeof(FormatName[0]); i++) {
		DXGI_FORMAT format = static_cast<DXGI_FORMAT>(i);
		wprintf_s(L"[%s]\n  ", FormatName[i]);
		
		UINT formatSupport = 0;
		result = device->CheckFormatSupport(format, &formatSupport);
		if (result == E_FAIL) {
			wprintf_s(L"Unsupported.\n");
			continue;
		} else if (FAILED(result)) {
			fwprintf_s(stderr, L"ID3D11Device::CheckFormatSupport() failed.\n");
			return 0;
		}

		wprintf_s(L"[%c ", (formatSupport & D3D11_FORMAT_SUPPORT_BUFFER) ? L'B' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER) ? L'V' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER) ? L'I' : L'-');
		wprintf_s(L"%c] ", (formatSupport & D3D11_FORMAT_SUPPORT_SO_BUFFER) ? L'S' : L'-');

		wprintf_s(L"[%c ", (formatSupport & D3D11_FORMAT_SUPPORT_TEXTURE1D) ? L'1' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D) ? L'2' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_TEXTURE3D) ? L'3' : L'-');
		wprintf_s(L"%c] ", (formatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D) ? L'C' : L'-');

		wprintf_s(L"[%c ", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_LOAD) ? L'L' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE) ? L'S' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON) ? L'C' : L'-');
		wprintf_s(L"%c] ", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT) ? L'*' : L'-');

		wprintf_s(L"[%c ", (formatSupport & D3D11_FORMAT_SUPPORT_MIP) ? L'M' : L'-');
		wprintf_s(L"%c] ", (formatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN) ? L'A' : L'-');

		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET) ? L'R' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_BLENDABLE) ? L'B' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL) ? L'D' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_CPU_LOCKABLE) ? L'C' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) ? L'M' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_DISPLAY) ? L'D' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT) ? L'C' : L'-');

		wprintf_s(L"[%c ", (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) ? L'R' : L'-');
		wprintf_s(L"%c] ", (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) ? L'L' : L'-');

		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_GATHER) ? L'G' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST) ? L'C' : L'-');
		wprintf_s(L"%c ", (formatSupport & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW) ? L'V' : L'-');
		wprintf_s(L"%c\n", (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON) ? L'C' : L'-');
	}

	return 0;
}

