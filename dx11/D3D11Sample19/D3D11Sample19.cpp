// D3D11Sample19.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <D3D11SampleCommon.h>

const size_t DATA_SIZE = 1920 * 1080 * 3;

Microsoft::WRL::ComPtr<ID3D11Device> device;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
Microsoft::WRL::ComPtr<ID3D11Buffer> buffer[2];
Microsoft::WRL::ComPtr<ID3D11Buffer> readBackBuffer;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[2];
Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unorderedAccessView[2];

struct ConstantBuffer {
	unsigned int dataCount;
	unsigned int padding[3];
};
ConstantBuffer constantBufferData;

HRESULT CreateDevice()
{
	HRESULT result = S_OK;
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL featureLevel;

	for (auto driverType : { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE }) {
		result = D3D11CreateDevice(
			nullptr,
			driverType,
			nullptr,
			0,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			&featureLevel,
			&deviceContext);
		if (SUCCEEDED(result)) {
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS options;
			device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &options, sizeof(options));
			if (options.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
				const wchar_t *typeString = nullptr;
				switch (driverType) {
				case D3D_DRIVER_TYPE_HARDWARE:	typeString = L"D3D_DRIVER_TYPE_HARDWARE";	break;
				case D3D_DRIVER_TYPE_WARP:		typeString = L"D3D_DRIVER_TYPE_WARP";		break;
				case D3D_DRIVER_TYPE_REFERENCE:	typeString = L"D3D_DRIVER_TYPE_REFERENCE";	break;
				default: typeString = L"(invalid)"; break;
				}
				wprintf_s(L"[%s]\n", typeString);
				return result;
			}
			device.Reset();
			deviceContext.Reset();
		}
	}

	return result;
}

HRESULT CreateShader()
{
	HRESULT result = S_OK;

	std::vector<uint8_t> csBlob;
	result = readFile(L"D3D11Sample19.cso", csBlob);
	if (FAILED(result)) {
		return result;
	}

	result = device->CreateComputeShader(csBlob.data(), csBlob.size(), nullptr, &computeShader);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateComputeShader() failed.");
	}

	return result;
}

HRESULT CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);

	HRESULT result = device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBuffer() failed.");
	}

	return result;
}

HRESULT CreateResource()
{
	std::array<float, DATA_SIZE> initialData;
	for (float &data : initialData) {
		data = 1.0f;
	}

	float sum = 0.0f;
	DWORD time = timeGetTime();
	for (int i = 0; i < initialData.size(); i++) {
		sum += initialData[i];
	}

	time = timeGetTime() - time;
	wprintf_s(L"[CPU] %u ms [SUM = %f]\n", time, sum);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = DATA_SIZE * sizeof(float);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = initialData.data();
	subResource.SysMemPitch = 0;
	subResource.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&bufferDesc, &subResource, &buffer[0]);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBuffer() failed.");
	}

	result = device->CreateBuffer(&bufferDesc, nullptr, &buffer[1]);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBuffer() failed.");
	}

	bufferDesc.ByteWidth = sizeof(float);
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.StructureByteStride = sizeof(float);

	result = device->CreateBuffer(&bufferDesc, nullptr, &readBackBuffer);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBuffer() failed.");
	}

	return result;
}

HRESULT CreateShaderResourceView()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	memset(&viewDesc, 0, sizeof(viewDesc));
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	viewDesc.Buffer.ElementWidth = DATA_SIZE;

	HRESULT result = S_OK;

	for (int i = 0; i < 2; i++) {
		result = device->CreateShaderResourceView(buffer[i].Get(), &viewDesc, &shaderResourceView[i]);
		if (FAILED(result)) {
			return TRACE_ERROR(result, L"ID3D11Device::CreateShaderResourceView() failed.");
		}
	}

	return result;
}

HRESULT ComputeShader()
{
	HRESULT result = S_OK;
	DWORD t0 = timeGetTime();

	deviceContext->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	deviceContext->CSSetShader(computeShader.Get(), nullptr, 0);

	unsigned int dataCount = DATA_SIZE;
	bool resourceFlag = false;
	ID3D11ShaderResourceView *nullShaderResourceView = nullptr;
	do {
		resourceFlag != resourceFlag;
		deviceContext->CSSetShaderResources(0, 1, &nullShaderResourceView);
		deviceContext->CSSetUnorderedAccessViews(0, 1, unorderedAccessView[resourceFlag ? 1 : 0].GetAddressOf(), nullptr);
		deviceContext->CSSetShaderResources(0, 1, shaderResourceView[resourceFlag ? 0 : 1].GetAddressOf());

		constantBufferData.dataCount = dataCount;
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		result = deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result)) {
			return TRACE_ERROR(result, L"ID3D11DeviceContext::Map() failed.");
		}
		memcpy(mappedResource.pData, &constantBufferData, sizeof(constantBufferData));
		deviceContext->Unmap(constantBuffer.Get(), 0);

	}


}


HRESULT CreateUnorderedAccessView()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	memset(&viewDesc, 0, sizeof(viewDesc));
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	viewDesc.Buffer.NumElements = DATA_SIZE;

	HRESULT result = S_OK;
	
	for (int i = 0; i < 2; i++) {
		result = device->CreateUnorderedAccessView(buffer[i].Get(), &viewDesc, &unorderedAccessView[i]);
		if (FAILED(result)) {
			return TRACE_ERROR(result, L"ID3D11Device::CreateUnorderedAccessView() failed.");
		}
	}

	return result;
}

int wmain(int argc, wchar_t* argv[])
{
	return 0;
}

