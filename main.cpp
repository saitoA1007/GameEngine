
#include<iostream>

#include"Source/Core/WindowsApp.h"
#include"Source/Core/DirectXCommon.h"
#include"Source/Core/GraphicsPipeline.h"
#include"Source/Core/TextureManager.h"

#include"Source/Common/LogManager.h"
#include"Source/Common/ResourceLeakChecker.h"
#include"Source/Common/CrashHandle.h"

#include"Source/3D/Camera.h"
#include"Source/3D/Model.h"
#include"Source/3D/DirectionalLight.h"

#include"Source/2D/ImGuiManager.h"

#include"Source/Math/TransformationMatrix.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

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
	GraphicsPipeline* graphicsPipeline = GraphicsPipeline::GetInstance();
	graphicsPipeline->Initialize(dxCommon->GetDevice(), logManager.get());

	// 3dを描画する処理の初期化
	Model::StaticInitialize(dxCommon->GetDevice(), dxCommon->GetCommandList());

	// ImGuiの初期化
	std::shared_ptr<ImGuiManager> imGuiManager = std::make_shared<ImGuiManager>();
	imGuiManager->Initialize(windowsApp.get(), dxCommon.get());

	// テクスチャの初期化
	std::shared_ptr<TextureManager> textureManager = std::make_shared<TextureManager>();
	textureManager->Initialize(dxCommon.get(), logManager.get());

	//=================================================================
	// 宣言と初期化
	//=================================================================

	// 画像をロード
	uint32_t uvCheckerGH = textureManager->Load("Resources/uvChecker.png");

	// カメラ
	Camera camera;
	camera.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} }, windowsApp->kWindowWidth, windowsApp->kWindowHeight);

	// モデル
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Model* plane = Model::CreateFromOBJ("axis.obj", "Axis/");
	plane->SetTransformationMatrix(camera.MakeWVPMatrix(worldMatrix));
	

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

		// ImGuiにフレームが始まる旨を伝える
		imGuiManager->BeginFrame();

		/// 更新処理

		// imgui
		ImGui::Begin("DebugWindow");
		ImGui::End();


		// ImGuiの受付終了
		imGuiManager->EndFrame();

		/// 描画処理

		// 描画前処理
		dxCommon->PreDraw(graphicsPipeline->GetRootSignature(), graphicsPipeline->GetPipelineState());

		plane->Draw(worldMatrix, directionalLight.GetResource(), &textureManager->GetTextureSrvHandlesGPU(uvCheckerGH), camera.GetVPMatrix());

		// ImGuiの描画処理
		imGuiManager->Draw();
		// 描画後処理
		dxCommon->PostDraw();
	}

	// テクスチャの解放
	textureManager->Finalize();

	// ImGuiの解放処理
	imGuiManager->Finalize();

	// GraphicsPipeline解放
	graphicsPipeline->Finalize();

	// DirectXCommonの解放処理
	dxCommon->Finalize();

	// WindowAppの解放
	windowsApp->BreakGameWindow();

	return 0;
}