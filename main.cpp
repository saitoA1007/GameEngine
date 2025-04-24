
#include"Source/Core/WindowsApp.h"
#include<iostream>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	// ウィンドウの作成
	std::shared_ptr<WindowsApp> windowsApp = std::make_shared<WindowsApp>();
	windowsApp->CreateGameWindow(L"CG2", 1280, 720);


	//=========================================================================
	// メインループ
	//=========================================================================

	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (windowsApp->ProcessMessage()) {
			break;
		}




	}


	// WindowAppの解放
	windowsApp->BreakGameWindow();

	return 0;
}