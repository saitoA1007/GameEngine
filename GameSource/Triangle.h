#pragma once
#include"EngineSource/3D/Model.h"
#include"EngineSource/3D/Camera.h"
#include"EngineSource/3D/WorldTransform.h"
#include"EngineSource/3D/Material.h"

class Triangle {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="textureHandle"></param>
	/// <param name="camera"></param>
	void Initialize(GameEngine::Model* model, const uint32_t textureHandle[2] , const GameEngine::Camera* camera);

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
	uint32_t textureHandle_[2][2] = {};

	// 選択しているテクスチャ番号
	int selectTextureNum1_ = 0;
	int selectTextureNum2_ = 0;

	// uvtextureを表示しているかを判断
	bool isUVtexture_[2] = { true,true };

	// 格納されているテクスチャ名
	const char* textureName1[2] = { "uvChecker","monsterBall" };
	const char* textureName2[2] = {"uvChecker","monsterBall"};

	// カメラ
	const GameEngine::Camera* camera_;

	// パーティクルモデル
	GameEngine::Model* triangleModel_ = nullptr;

	// ワールド行列
	GameEngine::WorldTransform worldTransform_[2];
	// SRTの各要素の格納
	Transform transform_[2];

	// 色
	Vector4 color_[2];

	// マテリアル
	GameEngine::Material material_[2];
};