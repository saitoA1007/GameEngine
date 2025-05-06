#pragma once
#include"Source/Math/Vector4.h"
#include"Source/Math/Matrix4x4.h"
#include<iostream>
#include <d3d12.h>
#include <wrl.h>

namespace GameEngine {

	class Material {
	public:
		struct MaterialData {
			Vector4 color;
			int32_t enableLighting;
			float padding[3];
			Matrix4x4 uvTransform;
		};

	public:

		static void StaticInitialize(ID3D12Device* device);

		void Initialize(const Vector4& color, const bool& isEnableLighting);

		// マテリアルリソースのゲッター
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetMaterialResource() const { return materialResource_; }

	public:

		/// <summary>
		/// 色を設定
		/// </summary>
		/// <param name="color"></param>
		void SetColor(Vector4 color) { materialData_->color = color; }

		/// <summary>
		/// ライトを適応させるかを設定
		/// </summary>
		/// <param name="isEnableLighting"></param>
		void SetEnableLighting(bool isEnableLighting) { materialData_->enableLighting = isEnableLighting; }

		/// <summary>
		/// UV行列を設定
		/// </summary>
		/// <param name="uvMatrix"></param>
		void SetUVMatrix(Matrix4x4 uvMatrix) { materialData_->uvTransform = uvMatrix; }

	private:

		static ID3D12Device* device_;

		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
		// マテリアルにデータを書き込む
		MaterialData* materialData_ = nullptr;
	};
}