
#include<iostream>

#include"Source/Core/WindowsApp.h"
#include"Source/Core/DirectXCommon.h"
#include"Source/Core/GraphicsPipeline.h"
#include"Source/Core/TextureManager.h"

#include"Source/Common/LogManager.h"
#include"Source/Common/ResourceLeakChecker.h"
#include"Source/Common/CrashHandle.h"

#include"Source/3D/Camera.h"
#include"Source/3D/DebugCamera.h"
#include"Source/3D/Model.h"
#include"Source/3D/DirectionalLight.h"

#include"Source/2D/ImGuiManager.h"
#include"Source/2D/Sprite.h"

#include"Source/Math/TransformationMatrix.h"

#include"Source/Input/InPut.h"

#include"Source/Audio/AudioManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	//============================================================
	// エンジン部分の初期化処理
	//============================================================

#pragma region 
	// 誰も補足しなかった場合に(Unhandled)、補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	// ログの初期化
	LogManager::Create();
	std::shared_ptr<LogManager> logManager = std::make_shared<LogManager>();

	// ウィンドウの作成
	std::shared_ptr<WindowsApp> windowsApp = std::make_shared<WindowsApp>();
	windowsApp->CreateGameWindow(L"CG2", 1280, 720);

	// リソースチェックのデバック
	D3DResourceLeakChecker leakCheck;

	// DirectXCommonの初期化
	std::shared_ptr<DirectXCommon> dxCommon = std::make_shared<DirectXCommon>();
	dxCommon->Initialize(windowsApp->GetHwnd(), windowsApp->kWindowWidth, windowsApp->kWindowHeight, logManager.get());

	// PSO設定の初期化
	std::shared_ptr<GraphicsPipeline> graphicsPipeline = std::make_shared<GraphicsPipeline>();
	graphicsPipeline->Initialize(dxCommon->GetDevice(), logManager.get());

	// ImGuiの初期化
	std::shared_ptr<ImGuiManager> imGuiManager = std::make_shared<ImGuiManager>();
	imGuiManager->Initialize(windowsApp.get(), dxCommon.get());

	// 入力処理を初期化
	std::shared_ptr<Input> input = std::make_shared<Input>();
	input->Initialize(hInstance, windowsApp->GetHwnd());

	// 音声の初期化
	std::shared_ptr<AudioManager> audioManager = std::make_shared<AudioManager>();
	audioManager->Initialize();

	// テクスチャの初期化
	std::shared_ptr<TextureManager> textureManager = std::make_shared<TextureManager>();
	textureManager->Initialize(dxCommon.get(), logManager.get());

	// 画像の初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), windowsApp->kWindowWidth, windowsApp->kWindowHeight);

	// 3dを描画する処理の初期化
	Model::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());
#pragma endregion

	//=================================================================
	// 宣言と初期化
	//=================================================================

	// 画像をロード
	uint32_t uvCheckerGH = textureManager->Load("Resources/uvChecker.png");
	uint32_t monsterBallGH = textureManager->Load("Resources/monsterBall.png");

	// カメラ
	Camera camera;
	camera.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} }, windowsApp->kWindowWidth, windowsApp->kWindowHeight);
	// デバックカメラ
	DebugCamera debugCamera;
	debugCamera.Initialize(windowsApp->kWindowWidth, windowsApp->kWindowHeight);

	// 3Dモデル
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Model* axis = Model::CreateFromOBJ("axis.obj", "Axis/");
	axis->SetTransformationMatrix(camera.MakeWVPMatrix(worldMatrix));
	
	// 平行根源
	DirectionalLight directionalLight;
	directionalLight.Initialize(dxCommon->GetDevice());
	Vector4 lightColor = directionalLight.GetLightColor();
	Vector3 lightDir = directionalLight.GetLightDir();
	float intensity = directionalLight.GetLightIntensity();

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

		// imgui
		ImGui::Begin("DebugWindow");
		// モデルの操作
		ImGui::ColorEdit3("ModelColor", &color.x);
		axis->SetColor(color);
		ImGui::DragFloat3("UVTranslate", &transform.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("UVrotate", &transform.rotate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("UVScale", &transform.scale.x, 0.01f, -10.0f, 10.0f);
		worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		// 光の色を変更
		ImGui::ColorEdit3("Light_Color", &lightColor.x);
		directionalLight.SetLightColor(lightColor);
		// 光の方向を変更
		ImGui::SliderFloat3("Light_Direction", &lightDir.x, -1.0f, 1.0f);
		directionalLight.SetLightDir(lightDir);
		// 光の強度を変更
		ImGui::SliderFloat("Light_Intensity", &intensity, 0.0f, 10.0f);
		directionalLight.SetLightIntensity(intensity);
		ImGui::End();

		//====================================================================
		// 描画処理
		//====================================================================

		// ImGuiの受付終了
		imGuiManager->EndFrame();
		// 描画前処理
		dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());


		// 3Dモデルを描画
		axis->Draw(worldMatrix, directionalLight.GetResource(), &textureManager->GetTextureSrvHandlesGPU(uvCheckerGH), debugCamera.GetVPMatrix());


		// ImGuiの描画処理
		imGuiManager->Draw();
		// 描画後処理
		dxCommon->PostDraw();
	}

	// 3Dモデルの解放処理
	delete axis;

	/// 解放処理

	// テクスチャの解放
	textureManager->Finalize();
	// ImGuiの解放処理
	imGuiManager->Finalize();
	// WindowAppの解放
	windowsApp->BreakGameWindow();
	return 0;
}