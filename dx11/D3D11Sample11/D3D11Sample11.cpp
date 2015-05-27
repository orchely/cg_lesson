#include "stdafx.h"
#include <D3D11SampleCommon.h>

struct ConstantBuffer
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT3 light;
	FLOAT dummy;
};

class Application
{
public:
	Application();
	~Application();

	HRESULT initialize(HINSTANCE hInstance);
	WPARAM run();

private:
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam);
	bool onIdle();
	HRESULT render();

	LRESULT onDestroy(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT onSize(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT onKeyDown(UINT msg, WPARAM wParam, LPARAM lParam);

	HRESULT initializeWindow();
	HRESULT initializeDirect3D();
	HRESULT initializeBackBuffer();
	HRESULT isDeviceRemoved();
	void cleanupDirect3D();

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	LONG m_width;
	LONG m_height;

	static const std::wstring m_title;
	static const std::wstring m_windowClass;

#if defined(_DEBUG)
	DWORD m_prevFpsTime;
	DWORD m_frameCount;
#endif

	bool m_stanbyMode;
	bool m_depthMode;

	std::array<FLOAT, 4> m_clearColor;
	DirectX::XMFLOAT3 m_lightPosition;

	struct ConstantBuffer m_constantBufferData;

	D3D_FEATURE_LEVEL m_featureLevel;
	D3D11_VIEWPORT m_viewPort;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};

const std::wstring Application::m_title{ L"Direct3D 11 Sample11" };
const std::wstring Application::m_windowClass{ L"D3D11D11" };

Application::Application() :
m_hInstance(nullptr),
m_hWnd(nullptr),
m_width(1280),
m_height(720),
#if defined(_DEBUG)
m_prevFpsTime(0),
m_frameCount(0),
#endif
m_stanbyMode(false),
m_depthMode(true),
m_clearColor({ { 0.0f, 0.125f, 0.3f, 1.0f } }),
m_lightPosition(3.0f, 3.0f, -3.0f),
m_constantBufferData(),
m_featureLevel(D3D_FEATURE_LEVEL_9_1),
m_viewPort(),
m_swapChain(),
m_device(),
m_immediateContext(),
m_renderTargetView(),
m_depthStencilTexture(),
m_depthStencilView(),
m_vertexShader(),
m_geometryShader(),
m_pixelShader(),
m_constantBuffer(),
m_blendState(),
m_rasterizerState(),
m_depthStencilState(),
m_texture(),
m_textureShaderResourceView(),
m_samplerState()
{
}

Application::~Application()
{
	cleanupDirect3D();
	UnregisterClassW(m_windowClass.c_str(), m_hInstance);
}

HRESULT Application::initialize(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	HRESULT result;
	result = initializeWindow();
	if (FAILED(result)) {
		return result;
	}

	return initializeDirect3D();
}

HRESULT Application::initializeWindow()
{
	WNDCLASSW wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Application::windowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_APPWORKSPACE);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_windowClass.c_str();

	if (RegisterClassW(&wc) == 0) {
		return TRACE_ERROR(GetLastError(), L"RegisterClassW() failed.");
	}

	RECT rect{ 0, 0, m_width, m_height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	if (CreateWindowExW(0, m_windowClass.c_str(), m_title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_width, m_height, nullptr, nullptr, m_hInstance, this) == nullptr) {
		return TRACE_ERROR(GetLastError(), L"CreateWindowExW() failed.");
	}

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	return S_OK;
}

HRESULT Application::initializeDirect3D()
{
	// ID3D11Device, ID3D11DeviceContext
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 3;
	swapChainDesc.BufferDesc.Width = m_width;
	swapChainDesc.BufferDesc.Height = m_height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#if defined(_DEBUG)
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif

	HRESULT result = S_OK;
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	for (auto driverType : { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE }) {
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			nullptr,
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&m_swapChain,
			&m_device,
			&m_featureLevel,
			&m_immediateContext);
		if (SUCCEEDED(result)) {
			break;
		}
	}

	if (FAILED(result)) {
		return TRACE_ERROR(result, L"D3D11CreateDeviceAndSwapChain() failed.");
	}

	// vertex shader
	std::vector<uint8_t> vsBlob;
	result = readFile(L"D3D11Sample11Vs.cso", vsBlob);
	if (FAILED(result)) {
		return result;
	}
	result = m_device->CreateVertexShader(vsBlob.data(), vsBlob.size(), nullptr, &m_vertexShader);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateVertexShader() failed.");
	}

	// geometry shader
	std::vector<uint8_t> gsBlob;
	result = readFile(L"D3D11Sample11Gs.cso", gsBlob);
	if (FAILED(result)) {
		return result;
	}
	result = m_device->CreateGeometryShader(gsBlob.data(), static_cast<DWORD>(gsBlob.size()), nullptr, &m_geometryShader);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateGeometryShader() failed.");
	}

	// pixel shader
	std::vector<uint8_t> psBlob;
	result = readFile(L"D3D11Sample11Ps.cso", psBlob);
	if (FAILED(result)) {
		return result;
	}
	result = m_device->CreatePixelShader(psBlob.data(), static_cast<DWORD>(psBlob.size()), nullptr, &m_pixelShader);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreatePixelShader() failed.");
	}

	// constant buffer
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	result = m_device->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBuffer() failed.");
	}

	// blend state
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = m_device->CreateBlendState(&blendDesc, &m_blendState);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateBlendState() failed.");
	}

	// rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	result = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateRasterizerState() failed.");
	}

	// depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0;
	depthStencilDesc.StencilWriteMask = 0;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateDepthStencilState() failed.");
	}

	// create texture
	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(textureDesc));
	textureDesc.Width = 128;
	textureDesc.Height = 128;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	result = m_device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CrateTexture2D() failed.");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;
	result = m_device->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, &m_textureShaderResourceView);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateShaderResourceView() failed.");
	}

	// write texture image by CPU
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_immediateContext->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11DeviceContext::Map() failed.");
	}

	UCHAR *texels = static_cast<UCHAR*>(mappedResource.pData);
	for (UINT row = 0; row < textureDesc.Height; row++) {
		float b = 1.5f * (2.0f * row - textureDesc.Height) / textureDesc.Height;
		UINT rowStart = row * mappedResource.RowPitch;
		for (UINT col = 0; col < textureDesc.Width; col++) {
			float a = 1.5f * (2.0f * col - textureDesc.Width) / textureDesc.Width - 0.5f;
			float x = 0, y = 0;
			int count = 0;
			while (count < 0x0f) {
				float xn = x * x - y * y + a;
				float yn = 2 * x * y + b;
				x = xn;
				y = yn;
				float z = xn * xn + yn * yn;
				if (z > 100.0f) {
					break;
				}
				count++;
			}
			UINT colStart = col * 4;
			texels[rowStart + colStart + 0] = static_cast<UCHAR>(count == 0x0f ? 255 : (count << 4));
			texels[rowStart + colStart + 1] = static_cast<UCHAR>(count == 0x0f ? 255 : 255 - (count << 4));
			texels[rowStart + colStart + 2] = static_cast<UCHAR>(count == 0x0f ? 255 : 0);
			texels[rowStart + colStart + 3] = 255;
		}
	}

	m_immediateContext->Unmap(m_texture.Get(), 0);

	// texture sampler
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 2;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	result = m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateSamplerState() failed.");
	}

	result = initializeBackBuffer();
	if (FAILED(result)) {
		return result;
	}

	return S_OK;
}

HRESULT Application::initializeBackBuffer()
{
	HRESULT result = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"IDXGISwapChain::GetBuffer() failed.");
	}

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);
	result = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateRenderTargetView() failed.");
	}

	D3D11_TEXTURE2D_DESC depthBufferDesc = backBufferDesc;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilTexture);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateTexture2D() failed.");
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvViewDesc;
	dsvViewDesc.Format = depthBufferDesc.Format;
	dsvViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvViewDesc.Flags = 0;
	dsvViewDesc.Texture2D.MipSlice = 0;
	result = m_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &dsvViewDesc, &m_depthStencilView);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateDepthStencilView() failed.");
	}

	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = static_cast<FLOAT>(m_width);
	m_viewPort.Height = static_cast<FLOAT>(m_height);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	DirectX::XMMATRIX mat = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(30.0f),
		static_cast<float>(backBufferDesc.Width) / static_cast<float>(backBufferDesc.Height),
		1.0f,
		20.0f);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.projection, DirectX::XMMatrixTranspose(mat));

	return S_OK;
}

HRESULT Application::isDeviceRemoved()
{
	HRESULT result = m_device->GetDeviceRemovedReason();
	if (FAILED(result)) {
		TRACE_ERROR(result, L"ID3D11Device::GetDeviceRemovedReason() failed.");
	}

	if (result == DXGI_ERROR_DEVICE_HUNG || result == DXGI_ERROR_DEVICE_RESET) {
		cleanupDirect3D();
		result = initializeDirect3D();
	}

	return result;
}

void Application::cleanupDirect3D()
{
	if (m_immediateContext != nullptr) {
		m_immediateContext->ClearState();
	}
	if (m_swapChain != nullptr) {
		m_swapChain->SetFullscreenState(FALSE, nullptr);
	}
}

bool Application::onIdle()
{
	if (m_device == nullptr) {
		return false;
	}

	if (FAILED(isDeviceRemoved())) {
		return false;
	}

	HRESULT result;

	if (m_stanbyMode) {
		result = m_swapChain->Present(0, DXGI_PRESENT_TEST);
		if (result != S_OK) {
			Sleep(100);
			return true;
		}
		m_stanbyMode = false;
		OutputDebugStringW(L"exit stanby mode\n");
	}

	result = render();
	if (result == DXGI_STATUS_OCCLUDED) {
		m_stanbyMode = true;
		OutputDebugStringW(L"enter stanby mode\n");
	}

	return true;
}

HRESULT Application::render()
{
	HRESULT result = S_OK;
	m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), m_clearColor.data());
	m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	DirectX::XMVECTORF32 eyePosition = { 0.0f, 5.0f, -5.0f, 1.0f };
	DirectX::XMVECTORF32 focusPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMVECTORF32 upDirection = { 0.0f, 1.0f, 0.0f, 1.0f };
	DirectX::XMMATRIX mat = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.view, DirectX::XMMatrixTranspose(mat));

	DirectX::XMVECTOR vec = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_lightPosition), mat);
	DirectX::XMStoreFloat3(&m_constantBufferData.light, vec);

	DirectX::XMMATRIX matY, matX;
	FLOAT rotate = static_cast<FLOAT>(DirectX::XM_PI * (timeGetTime() % 3000)) / 1500.0f;
	matY = DirectX::XMMatrixRotationY(rotate);
	rotate = static_cast<FLOAT>(DirectX::XM_PI * (timeGetTime() % 1500)) / 750.0f;
	matX = DirectX::XMMatrixRotationX(rotate);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.world, DirectX::XMMatrixTranspose(matY * matX));

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_immediateContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11DeviceContext::Map() failed.");
	}
	memcpy(mappedResource.pData, &m_constantBufferData, sizeof(ConstantBuffer));
	m_immediateContext->Unmap(m_constantBuffer.Get(), 0);

	m_immediateContext->IASetInputLayout(nullptr);
	m_immediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_immediateContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	m_immediateContext->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	m_immediateContext->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	m_immediateContext->RSSetViewports(1, &m_viewPort);
	m_immediateContext->RSSetState(m_rasterizerState.Get());

	m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	m_immediateContext->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	m_immediateContext->PSSetShaderResources(0, 1, m_textureShaderResourceView.GetAddressOf());
	m_immediateContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthMode ? m_depthStencilView.Get() : nullptr);

	FLOAT blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_immediateContext->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);
	m_immediateContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	m_immediateContext->Draw(36, 0);

	result = m_swapChain->Present(1, 0);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"IDXGISwapChain::Present() failed.");
	}

#if defined(_DEBUG)
	m_frameCount++;
	DWORD time = timeGetTime();
	DWORD delta = time - m_prevFpsTime;
	if (time - m_prevFpsTime >= 1000) {
		std::wstringstream ss;
		ss << m_frameCount * 1000 / delta << L"fps" << std::endl;
		OutputDebugStringW(ss.str().c_str());
		m_frameCount = 0;
		m_prevFpsTime = time;
	}
#endif

	return S_OK;
}

LRESULT CALLBACK Application::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) {
		CREATESTRUCTW *cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Application *instance = reinterpret_cast<Application*>(cs->lpCreateParams);
		instance->m_hWnd = hWnd;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance));
	}

	Application *instance = reinterpret_cast<Application*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (instance != nullptr) {
		return instance->windowProc(msg, wParam, lParam);
	}

	// fallback
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Application::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:	return onDestroy(msg, wParam, lParam);
	case WM_KEYDOWN:	return onKeyDown(msg, wParam, lParam);
	case WM_SIZE:		return onSize(msg, wParam, lParam);
	default:			return DefWindowProcW(m_hWnd, msg, wParam, lParam);
	}
}

LRESULT Application::onDestroy(UINT msg, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT Application::onKeyDown(UINT msg, WPARAM wParam, LPARAM lParam)
{
	HRESULT result;

	switch (wParam) {
	case VK_ESCAPE:
		PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case VK_F2:
		m_depthMode = !m_depthMode;
		break;
	case VK_F3:
		if (m_texture.Get() != nullptr) {
			DirectX::ScratchImage image;
			result = DirectX::CaptureTexture(m_device.Get(), m_immediateContext.Get(), m_texture.Get(), image);
			if (FAILED(result)) {
				TRACE_ERROR(result, L"DirectX::CaptureTexture() failed.");
				break;
			}
			result = DirectX::SaveToWICFile(*image.GetImages(), 0, GUID_ContainerFormatBmp, L"save_tex.bmp");
			if (FAILED(result)) {
				TRACE_ERROR(result, L"DirectX::SaveToWICFile() failed.");
				break;
			}
		}
		break;
	case VK_F4:
		if (m_renderTargetView.Get() != nullptr) {
			Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer;
			m_renderTargetView->GetResource(&backBuffer);

			DirectX::ScratchImage image;
			result = DirectX::CaptureTexture(m_device.Get(), m_immediateContext.Get(), backBuffer.Get(), image);
			if (FAILED(result)) {
				TRACE_ERROR(result, L"DirectX::CaptureTexture() failed.");
				break;
			}
			result = DirectX::SaveToWICFile(*image.GetImages(), 0, GUID_ContainerFormatBmp, L"save_back.bmp");
			if (FAILED(result)) {
				TRACE_ERROR(result, L"DirectX::SaveToWICFile() failed.");
				break;
			}
		}
		break;
	case VK_F5:
		if (m_swapChain != nullptr) {
			BOOL fullScreen;
			m_swapChain->GetFullscreenState(&fullScreen, nullptr);
			m_swapChain->SetFullscreenState(!fullScreen, nullptr);
		}
		break;
	case VK_F6:
		if (m_swapChain != nullptr) {
			DXGI_MODE_DESC desc;
			desc.Width = 800;
			desc.Height = 600;
			desc.RefreshRate.Numerator = 60;
			desc.RefreshRate.Denominator = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			result = m_swapChain->ResizeTarget(&desc);
			if (FAILED(result)) {
				TRACE_ERROR(result, L"IDXGISwapChain::ResizeTarget() failed.");
			}
		}
	default:
		break;
	}

	return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

LRESULT Application::onSize(UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_width = LOWORD(lParam);
	m_height = HIWORD(lParam);

	if (m_device != nullptr && wParam != SIZE_MINIMIZED) {
		m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_renderTargetView.Reset();
		m_swapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		initializeBackBuffer();
	}

	return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

WPARAM Application::run()
{
	MSG msg;
	do {
		if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			if (!onIdle()) {
				DestroyWindow(m_hWnd);
			}
		}
	} while (msg.message != WM_QUIT);

	return msg.wParam;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!DirectX::XMVerifyCPUSupport()) {
		return 0;
	}

	Application app;
	if (FAILED(app.initialize(hInstance))) {
		return 0;
	}

	return static_cast<int>(app.run());
}