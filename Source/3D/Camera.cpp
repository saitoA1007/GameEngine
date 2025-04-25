#include"Camera.h"
#include"Source/Math/MyMath.h"
using namespace GameEngine;

void Camera::Initialize(Transform transform, int kClientWidth, int kClientHeight) {
	// Matrixの初期化
	cameraMatrix_ = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix_ = InverseMatrix(cameraMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(kClientWidth) / static_cast<float>(kClientHeight), 0.1f, 100.0f);
	VPMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::SetCameraPosition(Transform transform) {
	cameraMatrix_ = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix_ = InverseMatrix(cameraMatrix_);
	VPMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

Matrix4x4 Camera::MakeWVPMatrix(Matrix4x4 worldMatrix) {
	WVPMatrix_ = Multiply(worldMatrix, Multiply(viewMatrix_, projectionMatrix_));
	return WVPMatrix_;
}

Matrix4x4 Camera::GetVPMatrix() {
	return VPMatrix_;
}

void  Camera::SetVPMatrix(Matrix4x4 VPMatrix) {
	this->VPMatrix_ = VPMatrix;
}