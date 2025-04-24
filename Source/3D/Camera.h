#pragma once
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/Vector3.h"
#include"Source/Math/MyMath.h"

class Camera {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Transform transform, int kClientWidth, int kClientHeight);

	/// <summary>
	/// カメラの位置を変更
	/// </summary>
	/// <param name="transform"></param>
	void SetCameraPosition(Transform transform);

	Matrix4x4 MakeWVPMatrix(Matrix4x4 worldMatrix);

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	Matrix4x4 GetVPMatrix();
private:

	Matrix4x4 cameraMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 VPMatrix_;
	Matrix4x4 WVPMatrix_;
};