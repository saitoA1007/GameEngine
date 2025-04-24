
#include<iostream>

#include"Source/Core/WindowsApp.h"
#include"Source/Core/DirectXCommon.h"

#include"Source/Common/LogManager.h"
#include"Source/Common/ResourceLeakChecker.h"


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ログの初期化
	LogManager::Create();
	std::shared_ptr<LogManager> logManager = std::make_shared<LogManager>();

	// ウィンドウの作成
	std::shared_ptr<WindowsApp> windowsApp = std::make_shared<WindowsApp>();
	windowsApp->CreateGameWindow(L"CG2", 1280, 720);

	// リソースチェックのデバック
	D3DResourceLeakChecker leakCheck;

	// DirectXCommonの初期化
	//std::shared_ptr<DirectXCommon> dxCommon = std::make_shared<DirectXCommon>();
	//dxCommon->Initialize(windowsApp->GetHwnd(), windowsApp->kWindowWidth, windowsApp->kWindowHeight, logManager.get());

	//=========================================================================
	// メインループ
	//=========================================================================

	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (windowsApp->ProcessMessage()) {
			break;
		}


		// 描画前処理
		//dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());


		// 描画後処理
		//dxCommon->PostDraw();
	}


	// DirectXCommonの解放処理
	//dxCommon->Finalize();

	// WindowAppの解放
	windowsApp->BreakGameWindow();

	return 0;
}