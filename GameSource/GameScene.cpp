#include"GameScene.h"
#include"EngineSource/2D/ImGuiManager.h"
#include"EngineSource/Math/MyMath.h"

using namespace GameEngine;

GameScene::~GameScene() {
	// パーティクルの解放
	delete particles_;
	delete paritcleModel_;
	// 三角形の解放
	delete triangle_;
	delete triangleModel_;
	// 天球モデルの解放
	delete skyDomeModel_;
	// スプライトの解放
	delete sprite_;
}

void GameScene::Initialize(GameEngine::TextureManager* textureManager, GameEngine::DirectXCommon* dxCommon) {

	// シーン
	scene_ = Scene::kTriangle;

	// DirectX機能を受け取る
	dxCommon_ = dxCommon;

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-25.0f} }, 1280, 720);
	// デバックカメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize({ 0.0f,0.0f,-25.0f }, 1280, 720);

	// 軸方向表示の初期化
	axisIndicator_ = std::make_unique<AxisIndicator>();
	axisIndicator_->Initialize("axis.obj");
	// 軸方向表示の画像
	axisTextureHandle_ = textureManager->Load("Resources/axis/axis.jpg");

	// 天球のテクスチャを生成
	skyDomeTextureHandle_ = textureManager->Load("Resources/white4x4.png");
	// 天球モデルを生成
	skyDomeModel_ = Model::CreateFromOBJ("Sky.obj", "SKyDome");
	// SkyDomeクラスを作成
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_->Initialize(skyDomeModel_, skyDomeTextureHandle_, camera_.get());

	// パーティクルテクスチャを作成
	particleTextureHandle_[0] = textureManager->Load("Resources/gradation.png");
	particleTextureHandle_[1] = textureManager->Load("Resources/split.png");
	// パーティクルのモデルを作成
	paritcleModel_ = Model::CreateFromOBJ("triangular.obj","Triangular");
	// particlesクラス
	particles_ = nullptr;

	// 三角形のテクスチャを作成
	triangleTextureHandle_[0] = textureManager->Load("Resources/uvChecker.png");
	triangleTextureHandle_[1] = textureManager->Load("Resources/monsterBall.png");
	// 三角形のモデルを作成
	triangleModel_ = Model::CreateTrianglePlane();
	// 三角形のクラスを作成
	triangle_ = new Triangle();
	triangle_->Initialize(triangleModel_, triangleTextureHandle_, camera_.get());

	// 平行光源の生成
	lightColor_ = { 0.349f, 0.305f, 0.807f, 1.0f };
	lightDir_ = { 0.1f,-0.9f,0.1f };
	intensity_ = 6.2f;
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(dxCommon->GetDevice(), lightColor_, lightDir_, intensity_);

	// 画像クラスを作成
	sprite_ = Sprite::Create({ 0.0f,0.0f }, { 1280.0f,720.0f }, { 0.133f,0.090f,0.247f,0.09f });
	spriteTextureHandle_ = textureManager->Load("Resources/blurring.png");
}

void GameScene::Update(GameEngine::Input* input) {

	// シーンの切り替え処理
	if (input->TriggerKey(DIK_SPACE)) {
		if (scene_ == Scene::kTriangle) {
			// シーンを切り替え
			scene_ = Scene::kParticle;

			// 三角形のクラスを削除
			delete triangle_;
			triangle_ = nullptr;

			// パーティクルクラスを生成
			particles_ = new Particles();
			particles_->Initialize(paritcleModel_, particleTextureHandle_, camera_.get());

		} else {
			// シーンを切り替え
			scene_ = Scene::kTriangle;

			// パーティクルクラスを削除
			delete particles_;
			particles_ = nullptr;

			// 三角形のクラスを生成
			triangle_ = new Triangle();
			triangle_->Initialize(triangleModel_, triangleTextureHandle_, camera_.get());
		}	
	}

	switch (scene_){
	case Scene::kTriangle:

		// 三角形の更新処理
		triangle_->Update();

		break;

	case Scene::kParticle:

#ifdef _DEBUG
		ImGui::Begin("DebugWindow");
		// 光の色を変更
		ImGui::ColorEdit3("LightColor", &lightColor_.x);
		directionalLight_->SetLightColor(lightColor_);
		// 光の方向を変更
		ImGui::SliderFloat3("LightDirection", &lightDir_.x, -1.0f, 1.0f);
		lightDir_ = Normalize(lightDir_);
		directionalLight_->SetLightDir(lightDir_);
		// 光の強度を変更
		ImGui::SliderFloat("LightIntensity", &intensity_, 0.0f, 10.0f);
		directionalLight_->SetLightIntensity(intensity_);
		ImGui::End();
#endif

		// 天球の更新処理
		skyDome_->Update();

		// パーティクルの更新処理
		particles_->Update();

		break;
	}

	// カメラ処理
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update(input);
		// デバックカメラの値をカメラに代入
		camera_->SetVPMatrix(debugCamera_->GetVPMatrix());
		// 軸方向表示の更新処理
		axisIndicator_->Update(debugCamera_->GetRotateMatrix());
	} else {
		// カメラの更新処理
		camera_->Update();
	}

#ifdef _DEBUG
	// カメラの切り替え処理
	if (input->TriggerKey(DIK_F)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
	}
#endif 
}

void GameScene::Draw() {

	switch (scene_){
	case Scene::kTriangle:
		// 三角形の描画処理
		triangle_->Draw();
		break;

	case Scene::kParticle:

		// 天球を描画
		skyDome_->Draw();

		// パーティクルの描画処理
		particles_->Draw(directionalLight_->GetResource());

		// スプライトを描画
		sprite_->Draw(spriteTextureHandle_);
		break;
	}

	// 軸を描画
	axisIndicator_->Draw(axisTextureHandle_);
}