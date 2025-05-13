#include"DebugCamera.h"
#include"EngineSource/Math/MyMath.h"
using namespace GameEngine;

void DebugCamera::Initialize(const Vector3& translate,int width, int height) {
	translate_ = translate;
	viewMatrix_ = InverseMatrix(MakeAffineMatrix(scale_, rotate_, translate_));
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	rotateMatrix_ = MakeIdentity4x4();
}

void DebugCamera::Update(Input* input) {

	// 右クリックで回転する処理
	if (input->IsPressMouse(1)) {
		Vector2 delta = input->GetMouseDelta();

		Matrix4x4 rotateX = MakeRotateXMatrix(-delta.y * 0.01f);
		Matrix4x4 rotateY = MakeRotateYMatrix(-delta.x * 0.01f);

		rotateMatrix_ = Multiply(rotateMatrix_, Multiply(rotateX, rotateY));
	}

	Vector3 move = { 0.0f, 0.0f, 0.0f };

	// マウスの中ボタンを押す時、移動出来る
	if (input->IsPressMouse(2)) {
		// X軸の移動
		if (input->GetMouseDelta().x > 0.0f) { move.x -= 1.0f; }
		if (input->GetMouseDelta().x < 0.0f) { move.x += 1.0f; }
		// Y軸の移動
		if (input->GetMouseDelta().y < 0.0f) { move.y -= 1.0f; }
		if (input->GetMouseDelta().y > 0.0f) { move.y += 1.0f; }
	} else {
		// 前後
		if (input->GetWheel() > 0.0f) {
			move.z += 1.0f; 
		}
		if (input->GetWheel() < 0.0f) { 
			move.z -= 1.0f; 
		}
	}

	// 正規化（斜め移動でもスピード一定）
	float length = std::sqrt(move.x * move.x + move.y * move.y + move.z * move.z);
	if (length > 0.0f) {
		move.x /= length;
		move.y /= length;
		move.z /= length;

		const float speed = 0.05f;

		// x,zだけ回す
		float rotatedX = move.x * cosf(rotate_.y) - move.z * sinf(rotate_.y);
		float rotatedZ = move.x * sinf(rotate_.y) + move.z * cosf(rotate_.y);

		// 移動
		translate_.x += rotatedX * speed;
		translate_.y += move.y * speed; // Yはそのまま
		translate_.z += rotatedZ * (speed + 0.1f);
	}

	// ワールド行列
	Matrix4x4 worldMatrix = Multiply(MakeTranslateMatrix(translate_),rotateMatrix_);

	// カメラの変更した内容を適用する処理
	viewMatrix_ = InverseMatrix(worldMatrix);
}

Matrix4x4 DebugCamera::GetVPMatrix() {
	return Multiply(viewMatrix_, projectionMatrix_);
}

Matrix4x4 DebugCamera::GetRotateMatrix() {
	return rotateMatrix_;
}