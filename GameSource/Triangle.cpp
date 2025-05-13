#include"Triangle.h"
#include<cassert>
#include"EngineSource/2D/ImGuiManager.h"

using namespace GameEngine;

void Triangle::Initialize(GameEngine::Model* model, const uint32_t textureHandle[2], const GameEngine::Camera* camera) {
	// NULLチェック
	assert(model);

	// モデルを取得
	triangleModel_ = model;
	// 拡縮、回転、位置を設定
	transform_[0] = {{4.0,4.0f,4.0f}, {0.0f,-0.7f, 0.0f}, {0.0f,0.0f,0.0f}};
	transform_[1] = { {4.0,4.0f,4.0f}, {0.0f,1.0f, 0.0f}, {0.0f,0.0f,0.0f} };

	// ワールド行列の初期化
	worldTransform_[0].Initialize(transform_[0]);
	worldTransform_[1].Initialize(transform_[1]);
	// テクスチャを取得
	textureHandle_[0][0] = textureHandle[0];
	textureHandle_[0][1] = textureHandle[1];
	textureHandle_[1][0] = textureHandle[0];
	textureHandle_[1][1] = textureHandle[1];
	// カメラを取得
	camera_ = camera;

	// 色を設定
	color_[0] = { 1.0f,1.0f,1.0f,1.0f };
	color_[1] = { 1.0f,1.0f,1.0f,1.0f };
	// マテリアルを設定
	material_[0].Initialize(color_[0], false);
	material_[1].Initialize(color_[1], false);
}

void Triangle::Update() {

	// 三角形の色を設定
	ImGui::Begin("Window");
	if(ImGui::CollapsingHeader("object1")) {
		// 色の変更
		ImGui::ColorEdit3("color1", &color_[0].x);
		material_[0].SetColor(color_[0]);
		// SRTの変更
		ImGui::DragFloat3("Translate1", &transform_[0].translate.x, 0.01f);
		ImGui::DragFloat3("Rotate1", &transform_[0].rotate.x, 0.01f);
		ImGui::DragFloat3("Scale1", &transform_[0].scale.x, 0.01f);
		worldTransform_[0].SetTransform(transform_[0]);
		// テクスチャの変更
		if (ImGui::Combo("Texture1", &selectTextureNum1_, textureName1, IM_ARRAYSIZE(textureName1))) {

			if (selectTextureNum1_ == 0) {
				isUVtexture_[0] = true;
			} else {
				isUVtexture_[0] = false;
			}
		}
	}

	if (ImGui::CollapsingHeader("object2")) {
		// 色の変更
		ImGui::ColorEdit3("color2", &color_[1].x);
		material_[1].SetColor(color_[1]);
		// SRTの変更
		ImGui::DragFloat3("Translate2", &transform_[1].translate.x, 0.01f);
		ImGui::DragFloat3("Rotate2", &transform_[1].rotate.x, 0.01f);
		ImGui::DragFloat3("Scale2", &transform_[1].scale.x, 0.01f);
		worldTransform_[1].SetTransform(transform_[1]);
		// テクスチャの変更
		if (ImGui::Combo("Texture2", &selectTextureNum2_, textureName2, IM_ARRAYSIZE(textureName2))) {

			if (selectTextureNum2_ == 0) {
				isUVtexture_[1] = true;
			} else {
				isUVtexture_[1] = false;
			}
		}
	}
	ImGui::Text("ChangeScene : SPACE");
	ImGui::End();

	// 行列の更新処理
	worldTransform_[0].UpdateTransformMatrix();
	worldTransform_[1].UpdateTransformMatrix();
}

void Triangle::Draw() {
	// 三角形の描画
	if (isUVtexture_[0]) {
		triangleModel_->Draw(worldTransform_[0], textureHandle_[0][0], camera_->GetVPMatrix(), &material_[0]);
	} else {
		triangleModel_->Draw(worldTransform_[0], textureHandle_[0][1], camera_->GetVPMatrix(), &material_[0]);
	}

	if (isUVtexture_[1]) {
		triangleModel_->Draw(worldTransform_[1], textureHandle_[1][0], camera_->GetVPMatrix(), &material_[1]);
	} else {
		triangleModel_->Draw(worldTransform_[1], textureHandle_[1][1], camera_->GetVPMatrix(), &material_[1]);
	}
}