#include"SkyDome.h"
#include<cassert>

using namespace GameEngine;

void SkyDome::Initialize(GameEngine::Model* model, const uint32_t& textureHandle, const GameEngine::Camera* camera) {

	// NULLチェック
	assert(model);

	// モデルを取得
	skyDomeModel_ = model;
	// ワールド行列を更新
	worldTransform_.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.1f,0.1f,0.1f} });
	// テクスチャを取得
	textureHandle_ = textureHandle;
	// カメラを取得
	camera_ = camera;

	// 色を設定
	skyDomeModel_->SetDefaultColor({ 0.0f,0.0078f,0.0235f,1.0f });
}

void SkyDome::Update() {

	// ワールド行列を更新
	worldTransform_.UpdateTransformMatrix();
}

void SkyDome::Draw() {
	// 天球を描画
	skyDomeModel_->Draw(worldTransform_, textureHandle_, camera_->GetVPMatrix());
}