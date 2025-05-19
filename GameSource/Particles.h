#pragma once
#include"EngineSource/3D/Model.h"
#include"EngineSource/3D/Camera.h"
#include"EngineSource/3D/WorldTransform.h"
#include"EngineSource/3D/Material.h"
#include<array>
#include<numbers>

#include"RandomGenerator.h"

class Particles {
public:
	// パーティクルデータ
	struct Particle {
		GameEngine::WorldTransform worldTransform; // ワールド行列
		GameEngine::Material material; // マテリアル
		Vector3 color; // 色
		Vector3 velocity; // 移動速度
		float thetaSpeed; // 回転速度
		Transform uvtransform; // uvのトラスフォーム
		Vector3 uvScroll; // uvのスクロール量
		uint32_t textureHandle; // テクスチャ
		bool isAlive = false; // 生存フラグ
		float AliveTimer = 4.0f; // 生存タイマー
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="textureHandle"></param>
	/// <param name="camera"></param>
	void Initialize(GameEngine::Model* model, const uint32_t textureHandle[2], const GameEngine::Camera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ID3D12Resource* directionalLightResource);

private:

	// テクスチャハンドル
	uint32_t textureHandle_[2] = {};

	// カメラ
	const GameEngine::Camera* camera_;

	// パーティクルモデル
	GameEngine::Model* particleModel_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 256;

	// パーティクル
	std::array<Particle, kNumParticles> particles_;

	// パーティクルの存在時間
	static inline const float kParticleMaxTime = 3.0f;

	// 発生間隔の管理
	int coolTimeCount_ = 0;

	// ランダムな値をとる
	RandomGenerator rand_;

	// エミッター
	Vector3 emitterPos_ = { 1.0f,0.0f,-5.0f };
	Vector3 emitterVelocity_ = { 0.0f,0.0f,0.04f };
	float emitterTheta_ = 0.0f;
	float emitterSpeed_ = (2.0f * std::numbers::pi_v<float>) / 32.0f;
	float emitterRadius_ = 12.0f;

	float emitterTimer_ = 0.0f;
	static inline const float kEmitterMaxTime = 8.0f;
	static inline const float kEmitterStartPos = -5.0f;
	static inline const float kEmitterEndPos = 18.0f;
private:

	/// <summary>
	/// エミッターの処理
	/// </summary>
	void EmitterUpdate();
};