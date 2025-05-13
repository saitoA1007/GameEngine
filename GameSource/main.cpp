#include"GameEngine.h"
#include<iostream>

#include"GameScene.h"

using namespace GameEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	//============================================================
	// エンジン部分の初期化処理
	//============================================================

#pragma region
	// 誰も補足しなかった場合に(Unhandled)、補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	// ログの初期化
	LogManager::Create();
	std::unique_ptr<LogManager> logManager = std::make_unique<LogManager>();

	// ウィンドウの作成
	std::unique_ptr<WindowsApp> windowsApp = std::make_unique<WindowsApp>();
	windowsApp->CreateGameWindow(L"CG2_LE2A_05_サイトウ_アオイ", 1280, 720);

	// リソースチェックのデバック
	D3DResourceLeakChecker leakCheck;

	// DirectXCommonの初期化
	std::unique_ptr<DirectXCommon> dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initialize(windowsApp->GetHwnd(), windowsApp->kWindowWidth, windowsApp->kWindowHeight, logManager.get());

	// PSO設定の初期化
	std::unique_ptr<GraphicsPipeline> graphicsPipeline = std::make_unique<GraphicsPipeline>();
	graphicsPipeline->Initialize(L"Resources/Shaders/Object3d.VS.hlsl", L"Resources/Shaders/Object3d.PS.hlsl",dxCommon->GetDevice(), logManager.get());

	// ImGuiの初期化
	std::unique_ptr<ImGuiManager> imGuiManager = std::make_unique<ImGuiManager>();
	imGuiManager->Initialize(windowsApp.get(), dxCommon.get());

	// 入力処理を初期化
	std::unique_ptr<Input> input = std::make_unique<Input>();
	input->Initialize(hInstance, windowsApp->GetHwnd());

	// 音声の初期化
	std::unique_ptr<AudioManager> audioManager = std::make_unique<AudioManager>();
	audioManager->Initialize();

	// テクスチャの初期化
	std::shared_ptr<TextureManager> textureManager = std::make_shared<TextureManager>();
	textureManager->Initialize(dxCommon.get(), logManager.get());

	// 画像の初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), textureManager.get(), windowsApp->kWindowWidth, windowsApp->kWindowHeight);
	// 3dを描画する処理の初期化
	Model::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), textureManager.get(), logManager.get());
	// 軸方向表示の初期化
	AxisIndicator::StaticInitialize(dxCommon->GetCommandList());
	// ワールドトランスフォームの初期化
	WorldTransform::StaticInitialize(dxCommon->GetDevice());
	// マテリアルの初期化
	Material::StaticInitialize(dxCommon->GetDevice());
#pragma endregion

	//=================================================================
	// 宣言と初期化
	//=================================================================

	// ゲームシーンのインスタンスを生成
	GameScene* gameScene = new GameScene();
	gameScene->Initialize(textureManager.get(), dxCommon.get());

	//=========================================================================
	// メインループ
	//=========================================================================

	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (windowsApp->ProcessMessage()) {
			break;
		}

		// キー入力の更新処理
		input->Update();
		// ImGuiにフレームが始まる旨を伝える
		imGuiManager->BeginFrame();

		//==================================================================
		// 更新処理
		//==================================================================

		// ゲームシーンの更新処理
		gameScene->Update(input.get());

		//====================================================================
		// 描画処理
		//====================================================================

		// ImGuiの受付終了
		imGuiManager->EndFrame();
		// 描画前処理
		dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());

		// ゲームシーンの描画処理
		gameScene->Draw();

		// ImGuiの描画処理
		imGuiManager->Draw();
		// 描画後処理
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;

	/// 解放処理

	// テクスチャの解放
	textureManager->Finalize();
	// ImGuiの解放処理
	imGuiManager->Finalize();
	// WindowAppの解放
	windowsApp->BreakGameWindow();
	return 0;
}