#include"GameEngine.h"
#include<iostream>

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
	graphicsPipeline->Initialize(dxCommon->GetDevice(), logManager.get());

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
	Sprite::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), windowsApp->kWindowWidth, windowsApp->kWindowHeight);
	// 3dを描画する処理の初期化
	Model::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), textureManager.get(), logManager.get());
	// 軸方向表示の初期化
	AxisIndicator::StaticInitialize(dxCommon->GetCommandList());
	// ワールドトランスフォームの初期化
	WorldTransform::StaticInitialize(dxCommon->GetDevice());
#pragma endregion

	//=================================================================
	// 宣言と初期化
	//=================================================================

	// 画像をロード
	//uint32_t uvCheckerGH = textureManager->Load("Resources/uvChecker.png");
	uint32_t axisGH = textureManager->Load("Resources/axis/axis.jpg");

	// 軸方向表示
	std::unique_ptr<AxisIndicator> axisIndicator = std::make_unique<AxisIndicator>();
	axisIndicator->Initialize("axis.obj");

	// カメラ
	Camera camera;
	camera.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-20.0f} }, windowsApp->kWindowWidth, windowsApp->kWindowHeight);
	// デバックカメラ
	DebugCamera debugCamera;
	debugCamera.Initialize({ 0.0f,0.0f,-10.0f }, windowsApp->kWindowWidth, windowsApp->kWindowHeight);

	// 3Dモデル
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	WorldTransform* worldTransform = new WorldTransform();
	worldTransform->Initialize(transform);
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	bool isAxisLightOn = false;
	Model* axis = Model::CreateFromOBJ("axis.obj", "axis");
	
	// 平行根源
	DirectionalLight* directionalLight = new DirectionalLight();
	directionalLight->Initialize(dxCommon->GetDevice());
	Vector4 lightColor = directionalLight->GetLightColor();
	Vector3 lightDir = directionalLight->GetLightDir();
	float intensity = directionalLight->GetLightIntensity();

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

		// デバックカメラの更新処理
		debugCamera.Update(input.get());

#ifdef _DEBUG
		// imgui
		ImGui::Begin("DebugWindow");
		// Axisの操作
		if (ImGui::TreeNode("Axis")) {
			ImGui::ColorEdit3("AxisModelColor", &color.x);
			axis->SetColor(color);
			ImGui::Checkbox("AxisLight", &isAxisLightOn);
			axis->SetLightOn(isAxisLightOn);
			ImGui::DragFloat3("AxisTranslate", &transform.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("Axisrotate", &transform.rotate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("AxisScale", &transform.scale.x, 0.01f, -10.0f, 10.0f);
			worldTransform->SetTransform(transform);
			ImGui::TreePop();
		}
		// 平行根源の操作
		if (ImGui::TreeNode("Light")) {
			// 光の色を変更
			ImGui::ColorEdit3("LightColor", &lightColor.x);
			directionalLight->SetLightColor(lightColor);
			// 光の方向を変更
			ImGui::SliderFloat3("LightDirection", &lightDir.x, -1.0f, 1.0f);
			directionalLight->SetLightDir(lightDir);
			// 光の強度を変更
			ImGui::SliderFloat("LightIntensity", &intensity, 0.0f, 10.0f);
			directionalLight->SetLightIntensity(intensity);
			ImGui::TreePop();
		}
		ImGui::End();
#endif

		// ワールド行列を更新
		worldTransform->UpdateTransformMatrix();

		// 軸を回転させる処理
		axisIndicator->Update(debugCamera.GetRotateMatrix());

		//====================================================================
		// 描画処理
		//====================================================================

		// ImGuiの受付終了
		imGuiManager->EndFrame();
		// 描画前処理
		dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());


		// Axisモデルを描画
		axis->DrawLight(directionalLight->GetResource());
		axis->Draw(*worldTransform, axisGH, debugCamera.GetVPMatrix());

		// 軸を描画
		axisIndicator->Draw(axisGH);

		// ImGuiの描画処理
		imGuiManager->Draw();
		// 描画後処理
		dxCommon->PostDraw();
	}

	// 3Dモデルの解放
	delete axis;
	// ワールド行列データを解放
	delete worldTransform;

	// ライトの解放
	delete directionalLight;

	/// 解放処理

	// テクスチャの解放
	textureManager->Finalize();
	// ImGuiの解放処理
	imGuiManager->Finalize();
	// WindowAppの解放
	windowsApp->BreakGameWindow();
	return 0;
}