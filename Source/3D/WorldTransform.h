#pragma once
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/Transform.h"
#include"Source/Math/TransformationMatrix.h"
#include <d3d12.h>
#include <wrl.h>

namespace GameEngine {

	class WorldTransform {
	public:
		WorldTransform() = default;
		~WorldTransform() = default;

		/// <summary>
		/// 静的初期化
		/// </summary>
		/// <param name="device"></param>
		static void StaticInitialize(ID3D12Device* device);

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="transform">Scale,Rotate,Translate : 各型Vector3</param>
		void Initialize(const Transform& transform);

		/// <summary>
		/// SRTを適応
		/// </summary>
		void UpdateTransformMatrix();

		// トラスフォームリソースのゲッター
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetTransformResource() const { return transformationMatrixResource_; }

	public:

		void SetScale(const Vector3& scale) { transform_.rotate = scale; }

		void SetRotateY(const float& rotateY) { transform_.rotate.y = rotateY; }
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		Vector3 GetRotate() { return transform_.rotate; }

		void SetTranslateY(const float& translateY) { transform_.translate.y = translateY; }
		void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
		Vector3 GetTranslate() const { return transform_.translate; }

		void SetTransform(const Transform transform) { transform_ = transform; }

		void SetWorldMatrix(const Matrix4x4 worldMatrix) { worldMatrix_ = worldMatrix; }
		Matrix4x4 GetWorldMatrix() const { return worldMatrix_; }

		/// <summary>
		/// WVP行列を作成
		/// </summary>
		/// <param name="VPMatrix"></param>
		void SetWVPMatrix(const Matrix4x4& VPMatrix);

	private:
		// コピー禁止
		WorldTransform(const WorldTransform&) = delete;
		WorldTransform& operator=(const WorldTransform&) = delete;

		static ID3D12Device* device_;

		Matrix4x4 worldMatrix_;
		Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

		Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
		// トラスフォームにデータを書き込む
		TransformationMatrix* transformationMatrixData_ = nullptr;
	};
}

