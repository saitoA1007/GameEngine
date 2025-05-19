#include"Particles.h"
#include<cassert>
#include<cmath>

#include"EngineSource/Math/MyMath.h"
#include"EngineSource/Math/EasingManager.h"
		
#include"EngineSource/2D/ImGuiManager.h"

using namespace GameEngine;

void Particles::Initialize(GameEngine::Model* model, const uint32_t textureHandle[2], const GameEngine::Camera* camera) {

	// NULLチェック
	assert(model);

	// モデルを取得
	particleModel_ = model;
	// テクスチャを取得
	textureHandle_[0] = textureHandle[0];
	textureHandle_[1] = textureHandle[1];
	// カメラを取得
	camera_ = camera;

	// ランダムな値を取る機能を初期化
	rand_.Initialize();

	// パーティクルの各要素を初期化
	for (auto& particle : particles_) {
		particle.worldTransform.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });
		particle.material.Initialize({1.0f,1.0f,1.0f,1.0f}, true);
		particle.uvtransform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, { 0.0f,0.0f,0.0f } };
		particle.material.SetUVMatrix(MakeAffineMatrix(particle.uvtransform.scale, particle.uvtransform.rotate, particle.uvtransform.translate));
		particle.velocity = {};
		particle.isAlive = false;
		particle.AliveTimer = kParticleMaxTime;
	}
}
	
void Particles::Update() {

	// エミッターの移動処理
	EmitterUpdate();

	// 発生間隔の管理
	if (coolTimeCount_ > 0) {
		coolTimeCount_--;
	} else {
		coolTimeCount_ = 1;
	}

	// パーティクルの発生管理
	if (coolTimeCount_ <= 0) {
		for (uint32_t i = 0; i < kNumParticles; ++i) {

			if (!particles_[i].isAlive) {

				// 生存フラグをtrueにする
				particles_[i].isAlive = true;

				// エミッターの位置を元にランダムな場所に出現させる
				particles_[i].worldTransform.SetTranslate(emitterPos_ + rand_.GetVector3(-0.5f, 0.5f));

				// 速度を求める処理
				// ベクトルを取る
				Vector3 move;
				move.x = emitterPos_.x - particles_[i].worldTransform.GetTranslate().x;
				move.y = emitterPos_.y - particles_[i].worldTransform.GetTranslate().y;
				move.z = emitterPos_.z - particles_[i].worldTransform.GetTranslate().z;
				float length = Length(move);
				// スピードを取る
				float speed = rand_.GetFloat(-0.04f, 0.04f);
				// speedが0の時は0,01fにする
				if (speed == 0.0f) {
					speed = 0.01f;
				}
				// 速度を代入
				particles_[i].velocity.x = move.x / length * speed;
				particles_[i].velocity.y = move.y / length * speed;
				particles_[i].velocity.z = move.z / length * speed;

				// 回転速度を求める
				float thetaSpeed = 0.01f * static_cast<float>(rand_.GetInt(-4, 4));
				// thetaSpeedが0の時は0,01fにする
				if (thetaSpeed == 0.0f) {
					thetaSpeed = 0.01f;
				}
				particles_[i].thetaSpeed = thetaSpeed;

				// 大きさを設定
				float scale = rand_.GetFloat(0.2f, 1.0f);
				particles_[i].worldTransform.SetScale(Vector3(scale, scale, scale));

				// 色の設定
				Vector3 color = {};
				int rd = rand_.GetInt(0, 1);
				// 番号によって使う色を返す
				if (rd == 0) {
					color = { 0.38f,0.866f,0.149f };
				} else if (rd == 1) {
					color = { 0.290f, 0.902f, 0.922f };
				}
				particles_[i].color = color;
				particles_[i].material.SetColor({ particles_[i].color.x,particles_[i].color.y,particles_[i].color.z,1.0f });

				// スクロール速度を設定
				int scrollSpeed = rand_.GetInt(0, 2);
				if (scrollSpeed == 0) {
					particles_[i].uvScroll = { 0.005f,0.005f,0.0f };
				} else if (scrollSpeed == 1) {
					particles_[i].uvScroll = { -0.005f,-0.005f,0.0f };
				} else if (scrollSpeed == 2) {
					particles_[i].uvScroll = { 0.005f,-0.005f,0.0f };
				}
				particles_[i].uvtransform.scale = { 0.5f,0.5f,1.0f };

				// テクスチャ座標
				particles_[i].uvtransform.translate = {0.0f,0.0f,0.0f};

				// テクスチャを決める
				particles_[i].textureHandle = textureHandle_[rand_.GetInt(0, 1)];
				if (i % 2 == 0) {
					break;
				}
				
			}
		}
	}

	// パーティクルの処理
	for (uint32_t i = 0; i < kNumParticles; ++i) {

		// 生存フラグがfalseだったら飛ばす
		if (!particles_[i].isAlive) continue;

		// 生存タイマーをカウント
		particles_[i].AliveTimer -= 1.0f / 60.0f;

		// 移動させる
		particles_[i].worldTransform.SetTranslate(particles_[i].worldTransform.GetTranslate() + particles_[i].velocity);

		// 回転処理
		particles_[i].worldTransform.SetRotate(particles_[i].worldTransform.GetRotate() + Vector3(particles_[i].thetaSpeed, particles_[i].thetaSpeed, particles_[i].thetaSpeed));

		// 色の処理
		particles_[i].material.SetAplha(Lerp(0.0f,1.0f, EaseInOut(particles_[i].AliveTimer / 4.0f)));

		// UVを動かす処理
		particles_[i].uvtransform.translate += particles_[i].uvScroll;
		particles_[i].material.SetUVMatrix(MakeAffineMatrix(particles_[i].uvtransform.scale, particles_[i].uvtransform.rotate, particles_[i].uvtransform.translate));

		// 生存タイマーが0の時初期化する
		if (particles_[i].AliveTimer <= 0.0f) {

			particles_[i].velocity = {};
			particles_[i].isAlive = false;
			particles_[i].AliveTimer = kParticleMaxTime;
		}
	}

	// ワールド行列の更新
	for (auto& particle : particles_) {
		// 生存フラグがtrueでなければ更新しない
		if (!particle.isAlive) continue;
		particle.worldTransform.UpdateTransformMatrix();
	}
}

void Particles::Draw(ID3D12Resource* directionalLightResource) {
	// パーティクルの描画
	for (auto& particle : particles_) {
		// 生存フラグがtrueでなければ描画しない
		if (!particle.isAlive) continue;
		particleModel_->DrawLight(directionalLightResource);
		particleModel_->Draw(particle.worldTransform, particle.textureHandle, camera_->GetVPMatrix(), &particle.material);
	}
}

void Particles::EmitterUpdate() {

	// 角度を足していく処理
	emitterTheta_ += emitterSpeed_;

	// emitterの移動を補間するための処理
	if (emitterTimer_ < 1.0f) {
		emitterTimer_ += 1.0f / (60.0f * kEmitterMaxTime);
	} else {
		emitterTimer_ = 0.0f;
	}

	// 半径を補間する処理
	emitterRadius_ = Lerp(12.0f, 0.0f, EaseInOut(emitterTimer_));

	// 回転させる処理
	emitterPos_.x = std::cosf(emitterTheta_) * emitterRadius_;
	emitterPos_.y = std::sinf(emitterTheta_) * emitterRadius_;
	// 補間を適応する
	emitterPos_.z = Lerp(kEmitterStartPos, kEmitterEndPos, EaseInOut(emitterTimer_));
}