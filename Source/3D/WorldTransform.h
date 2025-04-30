#pragma once
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/Transform.h"

namespace GameEngine {

	class WorldTransform {
	public:
		WorldTransform() = default;
		~WorldTransform() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="transform">SRTを入力</param>
		void Initialize(const Transform& transform);

		/// <summary>
		/// SRTを適応
		/// </summary>
		void UpdateTransformMatrix();

	public:

		void SetScale(const Vector3& scale) { transform_.rotate = scale; }
		void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
		void SetTranslate(const Vector3& translate) { transform_.rotate = translate; }

		void SetTransform(const Transform transform) { transform_ = transform; }
		void SetWorldMatrix(const Matrix4x4 worldMatrix) { this->worldMatrix_ = worldMatrix; }

		Matrix4x4 GetWorldMatrix() const { return worldMatrix_; }

	private:
		// コピー禁止
		WorldTransform(const WorldTransform&) = delete;
		WorldTransform& operator=(const WorldTransform&) = delete;

		Matrix4x4 worldMatrix_;
		Transform transform_;
	};
}

