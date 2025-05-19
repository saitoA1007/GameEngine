#pragma once
#include"EngineSource/Input/InPut.h"
#include"EngineSource/Core/TextureManager.h"
#include"EngineSource/3D/Camera.h"
#include"EngineSource/3D/DebugCamera.h"
#include"EngineSource/3D/AxisIndicator.h"
#include"EngineSource/3D/DirectionalLight.h"
#include"EngineSource/2D/Sprite.h"

#include"Particles.h"
#include"Triangle.h"
#include"SkyDome.h"

enum class Scene {
	kTriangle,
	kParticle,
};

class GameScene {
public:

	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureManager"></param>
	void Initialize(GameEngine::TextureManager* textureManager, GameEngine::DirectXCommon* dxCommon);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input"></param>
	void Update(GameEngine::Input * input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	GameEngine::DirectXCommon* dxCommon_;

	// 軸方向モデル用のテクスチャハンドル
	uint32_t axisTextureHandle_ = 0u;
	// 軸方向表示
	std::unique_ptr<GameEngine::AxisIndicator> axisIndicator_;

	// カメラ
	std::unique_ptr<GameEngine::Camera> camera_;
	// デバックカメラ
	std::unique_ptr<GameEngine::DebugCamera> debugCamera_;
	// デバックカメラの有効の判定
	bool isDebugCameraActive_ = false;

	// 天球のクラス
	std::unique_ptr<SkyDome> skyDome_;
	// 天球のモデル
	GameEngine::Model* skyDomeModel_ = nullptr;
	// 天球のテクスチャ
	uint32_t skyDomeTextureHandle_ = 0u;

	// パーティクルのクラス
	Particles* particles_ = nullptr;
	// パーティクルのモデル
	GameEngine::Model* paritcleModel_ = nullptr;
	// パーティクルテクスチャ
	uint32_t particleTextureHandle_[2] = {};

	// 三角形のクラス
	Triangle* triangle_ = nullptr;
	// 三角形のモデル
	GameEngine::Model* triangleModel_ = nullptr;
	// 三角形のテクスチャ
	uint32_t triangleTextureHandle_[2] = { 0u,0u };

	// 平行光源
	std::unique_ptr<GameEngine::DirectionalLight> directionalLight_;
	Vector4 lightColor_;
	Vector3 lightDir_;
	float intensity_;

	// 画像
	GameEngine::Sprite* sprite_ = nullptr;
	// 画像テクスチャ
	uint32_t spriteTextureHandle_ = 0u;

	// 描画の切り替え
	Scene scene_;
};