#pragma once
#include"EngineSource/Math/Matrix4x4.h"
#include"EngineSource/Math/Vector3.h"
#include"EngineSource/Math/Transform.h"

namespace GameEngine {

	class Camera {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="transform">Scale,Rotate,Translate : 各型Vector3</param>
		/// <param name="kClientWidth">画面横幅</param>
		/// <param name="kClientHeight">画面縦幅</param>
		void Initialize(Transform transform, int kClientWidth, int kClientHeight);

		/// <summary>
		/// カメラの更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// カメラの位置を変更
		/// </summary>
		/// <param name="transform"></param>
		void SetCameraPosition(Transform transform);

	public:

		/// <summary>
		/// モデルをカメラ座標に変換する処理
		/// </summary>
		/// <param name="worldMatrix"></param>
		/// <returns></returns>
		Matrix4x4 MakeWVPMatrix(Matrix4x4 worldMatrix);

		Matrix4x4 GetViewMatrix() { return viewMatrix_; }
		void SetViewMatrix(const Matrix4x4& viewMatrix);

		Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }
		void SetProjectionMatrix(float fovY, int kClientWidth, int kClientHeight, float near, float far);

		Matrix4x4 GetVPMatrix() const { return VPMatrix_; }
		void SetVPMatrix(Matrix4x4 VPMatrix) { VPMatrix_ = VPMatrix; }
		
	private:

		Matrix4x4 cameraMatrix_;
		Matrix4x4 viewMatrix_;
		Matrix4x4 projectionMatrix_;
		Matrix4x4 VPMatrix_;
		Matrix4x4 WVPMatrix_;
	};
}