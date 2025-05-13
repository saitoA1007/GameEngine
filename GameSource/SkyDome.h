#pragma once
#include"EngineSource/3D/Model.h"
#include"EngineSource/3D/Camera.h"
#include"EngineSource/3D/WorldTransform.h"

class SkyDome {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="textureHandle"></param>
	/// <param name="camera"></param>
	void Initialize(GameEngine::Model* model, const uint32_t& textureHandle,const GameEngine::Camera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// ワールド行列
	GameEngine::WorldTransform worldTransform_;

	// 天球モデル
	GameEngine::Model* skyDomeModel_ = nullptr;

	// カメラ
	const GameEngine::Camera* camera_;
};
