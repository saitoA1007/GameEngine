
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
	windowsApp->CreateGameWindow(L"CG2_LE2A_05_サイトウ_アオイ", 1280, 720);

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
	Model::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList(), logManager.get());
#pragma endregion

	//=================================================================
	// 宣言と初期化
	//=================================================================

	// 画像をロード
	uint32_t uvCheckerGH = textureManager->Load("Resources/uvChecker.png");
	uint32_t monsterBallGH = textureManager->Load("Resources/monsterBall.png");
	uint32_t cubeGH = textureManager->Load("Resources/cube/cube.jpg");

	// カメラ
	Camera camera;
	camera.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-20.0f} }, windowsApp->kWindowWidth, windowsApp->kWindowHeight);
	// デバックカメラ
	DebugCamera debugCamera;
	debugCamera.Initialize(windowsApp->kWindowWidth, windowsApp->kWindowHeight);

	// 3Dモデル
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	bool isAxisLightOn = false;
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Model* axis = Model::CreateFromOBJ("axis.obj", "Axis/");
	axis->SetTransformationMatrix(camera.MakeWVPMatrix(worldMatrix));

	// 3Dモデル
	Transform transformCube{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{2.0f,0.0f,0.0f} };
	Vector4 colorCube = { 1.0f,1.0f,1.0f,1.0f };
	bool isCubeLightOn = false;
	Matrix4x4 worldMatrixCube = MakeAffineMatrix(transformCube.scale, transformCube.rotate, transformCube.translate);
	Model* cube = Model::CreateFromOBJ("cube.obj", "cube/");
	cube->SetTransformationMatrix(camera.MakeWVPMatrix(worldMatrixCube));
	
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
			worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			ImGui::TreePop();
		}
		// Cubeの操作
		if (ImGui::TreeNode("Cube")) {
			ImGui::ColorEdit3("\nCubeModelColor", &colorCube.x);
			cube->SetColor(colorCube);
			ImGui::Checkbox("CubeLight", &isCubeLightOn);
			cube->SetLightOn(isCubeLightOn);
			ImGui::DragFloat3("CubeTranslate", &transformCube.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("Cuberotate", &transformCube.rotate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("CubeScale", &transformCube.scale.x, 0.01f, -10.0f, 10.0f);
			worldMatrixCube = MakeAffineMatrix(transformCube.scale, transformCube.rotate, transformCube.translate);
			ImGui::TreePop();
		}
		// 平行根源の操作
		if (ImGui::TreeNode("Light")) {
			// 光の色を変更
			ImGui::ColorEdit3("Light_Color", &lightColor.x);
			directionalLight->SetLightColor(lightColor);
			// 光の方向を変更
			ImGui::SliderFloat3("Light_Direction", &lightDir.x, -1.0f, 1.0f);
			directionalLight->SetLightDir(lightDir);
			// 光の強度を変更
			ImGui::SliderFloat("Light_Intensity", &intensity, 0.0f, 10.0f);
			directionalLight->SetLightIntensity(intensity);
			ImGui::TreePop();
		}
		ImGui::End();
#endif

		//====================================================================
		// 描画処理
		//====================================================================

		// ImGuiの受付終了
		imGuiManager->EndFrame();
		// 描画前処理
		dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());


		// Axisモデルを描画
		axis->Draw(worldMatrix, directionalLight->GetResource(), &textureManager->GetTextureSrvHandlesGPU(uvCheckerGH), debugCamera.GetVPMatrix());

		// Cubeモデルを描画
		cube->Draw(worldMatrixCube, directionalLight->GetResource(), &textureManager->GetTextureSrvHandlesGPU(cubeGH), debugCamera.GetVPMatrix());


		// ImGuiの描画処理
		imGuiManager->Draw();
		// 描画後処理
		dxCommon->PostDraw();
	}

	// 3Dモデルの解放
	delete axis;
	delete cube;

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