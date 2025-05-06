#include"AxisIndicator.h"
#include"Source/Math/MyMath.h"
using namespace GameEngine;

ID3D12GraphicsCommandList* AxisIndicator::commandList_ = nullptr;

void AxisIndicator::StaticInitialize(ID3D12GraphicsCommandList* commandList) {

	commandList_ = commandList;
}

void AxisIndicator::Initialize(const std::string& modelName) {
	model_ = std::unique_ptr<Model>(Model::CreateFromOBJ(modelName, "axis"));

	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };
	worldTransform_.Initialize(transform_);

	camera_.Initialize({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} }, 160, 90);
	camera_.SetProjectionMatrix(0.45f, 160, 90, 0.1f, 20.0f);

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(160);
	viewport_.Height = static_cast<float>(90);
	viewport_.TopLeftX = 1280.0f - 160.0f;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 1280 - 160;
	scissorRect_.right = static_cast<int>(1280.0f + 160.0f);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<int>(90);
}

void AxisIndicator::Update(const Matrix4x4& rotate) {
	// モデルにカメラの回転を適応
	Matrix4x4 tmp;
	tmp = Multiply(MakeTranslateMatrix({ 0.0f,0.0f,-10.0f }), rotate);
	camera_.SetViewMatrix(InverseMatrix(tmp));

	// カメラの更新処理
	camera_.Update();
}

void AxisIndicator::Draw(const uint32_t& textureHandle) {
	// 画面左上に表示されるようにする
	commandList_->RSSetViewports(1, &viewport_); // Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_); // Scirssorを設定
	// 軸を表示するモデルを描画
	model_->Draw(worldTransform_, textureHandle, camera_.GetVPMatrix());
}