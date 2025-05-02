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
		/// 初期化
		/// </summary>
		/// <param name="transform">Scale,Rotate,Translate : 各型Vector3</param>
		void Initialize(const Transform& transform, ID3D12Device* device);

		/// <summary>
		/// ワールド行列の更新処理
		/// </summary>
		void UpdateTransformMatrix();

		// トラスフォームリソースのゲッター
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetTransformResource() const { return transformationMatrixResource_; }

	public:

		void SetScale(const Vector3& scale) { transform_.rotate = scale; }
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		void SetTranslate(const Vector3& translate) { transform_.rotate = translate; }
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

		Matrix4x4 worldMatrix_;
		Transform transform_;

		Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
		TransformationMatrix* transformationMatrixData_ = nullptr;
	};
}

