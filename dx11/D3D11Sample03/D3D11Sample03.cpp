#include "stdafx.h"
#include <trace_error.h>

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

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	LONG m_width;
	LONG m_height;

	static const std::wstring m_title;
	static const std::wstring m_windowClass;

	bool m_stanbyMode;
	std::array<FLOAT, 4> m_clearColor;

	D3D_FEATURE_LEVEL m_featureLevel;
	D3D11_VIEWPORT m_viewPort;
	ATL::CComPtr<IDXGIFactory> m_factory;
	ATL::CComPtr<IDXGISwapChain> m_swapChain;
	ATL::CComPtr<ID3D11Device> m_device;
	ATL::CComPtr<ID3D11DeviceContext> m_immediateContext;
	ATL::CComPtr<ID3D11RenderTargetView> m_renderTargetView;
};

const std::wstring Application::m_title{ L"Direct3D 11 Sample03" };
const std::wstring Application::m_windowClass{ L"D3D11D03" };

Application::Application() :
m_hInstance(nullptr),
m_hWnd(nullptr),
m_width(1280),
m_height(720),
m_stanbyMode(false),
m_clearColor({ 0.0f, 0.125f, 0.3f, 1.0f }),
m_featureLevel(D3D_FEATURE_LEVEL_9_1),
m_viewPort(),
m_factory(),
m_swapChain(),
m_device(),
m_immediateContext(),
m_renderTargetView()
{
}

Application::~Application()
{
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
	if (m_immediateContext != nullptr) {
		m_immediateContext->ClearState();
	}

	DXGI_SWAP_CHAIN_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = m_width;
	desc.BufferDesc.Height = m_height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = m_hWnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#if defined(_DEBUG)
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif

	std::array<D3D_FEATURE_LEVEL, 3> featureLevels = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	HRESULT result = S_OK;
	IDXGISwapChain *swapChain = nullptr;
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *immediateContext = nullptr;

	for (auto driverType : { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE }) {
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			nullptr,
			createDeviceFlags,
			featureLevels.data(),
			static_cast<UINT>(featureLevels.size()),
			D3D11_SDK_VERSION,
			&desc,
			&swapChain,
			&device,
			&m_featureLevel,
			&immediateContext);
		if (SUCCEEDED(result)) {
			break;
		}
	}

	if (FAILED(result)) {
		return TRACE_ERROR(result, L"D3D11CreateDeviceAndSwapChain() failed.");
	}

	m_swapChain.Attach(swapChain);
	m_device.Attach(device);
	m_immediateContext.Attach(immediateContext);

	result = initializeBackBuffer();
	if (FAILED(result)) {
		return result;
	}

	IDXGIFactory *factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"CreateDXGIFactory() failed.");
	}
	m_factory.Attach(factory);

	result = m_factory->MakeWindowAssociation(m_hWnd, 0);
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"IDXGIFactory::MakeWindowAssociation() failed.");
	}

	return S_OK;
}

HRESULT Application::initializeBackBuffer()
{
	HRESULT result = S_OK;
	ID3D11Texture2D *backBuffer = nullptr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"IDXGISwapChain::GetBuffer() failed.");
	}

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	ID3D11RenderTargetView *renderTargetView = nullptr;
	result = m_device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();
	if (FAILED(result)) {
		return TRACE_ERROR(result, L"ID3D11Device::CreateRenderTargetView() failed.");
	}
	m_renderTargetView.Attach(renderTargetView);

	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = static_cast<FLOAT>(m_width);
	m_viewPort.Height = static_cast<FLOAT>(m_height);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	return S_OK;
}

HRESULT Application::isDeviceRemoved()
{
	HRESULT result = m_device->GetDeviceRemovedReason();
	if (FAILED(result)) {
		TRACE_ERROR(result, L"ID3D11Device::GetDeviceRemovedReason() failed.");
	}

	if (result == DXGI_ERROR_DEVICE_HUNG || result == DXGI_ERROR_DEVICE_RESET) {
		result = initializeDirect3D();
	}

	return result;
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
		FLOAT c = m_clearColor[0];
		m_clearColor[0] = m_clearColor[1];
		m_clearColor[1] = m_clearColor[2];
		m_clearColor[2] = m_clearColor[3];
		m_clearColor[3] = c;
		OutputDebugStringW(L"enter stanby mode\n");
	}

	return true;
}

HRESULT Application::render()
{
	HRESULT result = S_OK;
	m_immediateContext->ClearRenderTargetView(m_renderTargetView, m_clearColor.data());
	m_immediateContext->RSSetViewports(1, &m_viewPort);
	m_immediateContext->OMSetRenderTargets(1, &m_renderTargetView.p, nullptr);
	result = m_swapChain->Present(0, 0);
	if (FAILED(result)) {
		TRACE_ERROR(result, L"IDXGISwapChain::Present() failed.");
	}
	return result;
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
	switch (msg)
	{
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
	m_width = HIWORD(lParam);
	m_height = LOWORD(lParam);

	if (m_device != nullptr && wParam != SIZE_MINIMIZED) {
		m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_renderTargetView.Release();
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
		} else {
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

	Application app;
	if (FAILED(app.initialize(hInstance))) {
		return 0;
	}

	return static_cast<int>(app.run());
}