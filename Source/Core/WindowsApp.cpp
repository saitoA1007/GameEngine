#include"WindowsApp.h"
#include"externals/imgui/imgui.h"
using namespace GameEngine;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowsApp::CreateGameWindow(const std::wstring& title, int32_t kClientWidth, int32_t kClientHeight) {

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	/// ウィンドウクラスの作成
	// ウィンドウプロシージャ
	wc_.lpfnWndProc = &WindowProc;
	// ウィンドウクラス名
	wc_.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc_);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	wrc_ = { 0,0,kClientWidth,kClientHeight };

	// クライアント領域を元に実際のサイズをwrcを変更してもらう
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	hwnd_ = CreateWindow(
		wc_.lpszClassName,      // 利用するクラス名
		title.c_str(),          // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,    // よく見るウィンドウスタイル
		CW_USEDEFAULT,          // 表示X座標(Windowに任せる)
		CW_USEDEFAULT,          // 表示Y座標(WindowOSに任せる)
		wrc_.right - wrc_.left, // ウィンドウ横幅
		wrc_.bottom - wrc_.top, // ウィンドウ縦幅
		nullptr,                // 親ウィンドウハンドル
		nullptr,                // メニューハンドル
		wc_.hInstance,          // インスタンスハンドル
		nullptr);               // オプション

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

bool WindowsApp::ProcessMessage() {

	// falseならそのまま処理、trueなら終了
	while (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		if (msg_.message == WM_QUIT) {
			return true;
		}
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}
	return false;
}

void WindowsApp::BreakGameWindow() {
	// COMの終了処理
	CoUninitialize();

	CloseWindow(hwnd_);
}