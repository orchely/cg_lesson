#include "stdafx.h"
#include "trace_error.h"

HINSTANCE g_hInstance = nullptr;
HWND g_hWindow = nullptr;

WCHAR g_szWndClass[] = L"D3D11S01";

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


HRESULT InitApp(HINSTANCE hInst)
{
	g_hInstance = hInst;

	WNDCLASSW wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = g_szWndClass;

	if (RegisterClassW(&wc) == 0) {
		return TRACE_ERROR(GetLastError(), L"InitApp");
	}

	RECT rect{ 0, 0, 640, 480 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	g_hWindow = CreateWindowExW(0, g_szWndClass, L"Direct3D 11 Sample01", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInst, nullptr);
	if (g_hWindow == nullptr) {
		return TRACE_ERROR(GetLastError(), L"InitApp");
	}

	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;

	switch (msg)
	{
	case WM_DESTROY:
		// Direct3Dの終了処理
		//CleanupDirect3D();
		// ウインドウを閉じる
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;

	case WM_KEYDOWN:
		// キー入力の処理
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]キーでウインドウを閉じる
			PostMessageW(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	}

	// デフォルト処理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	TRACE_ERROR(0x80070005, L"");

	HRESULT hr = InitApp(hInstance);
	if (FAILED(hr)) {
		return 0;
	}

	// メッセージ ループ
	MSG msg;
	do
	{
		if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			//// アイドル処理
			//if (!AppIdle())
			//	// エラーがある場合，アプリケーションを終了する
			//	DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	return static_cast<int>(msg.wParam);
}