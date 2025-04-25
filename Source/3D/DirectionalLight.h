#pragma once
#include"Source/Math/Vector4.h"
#include"Source/Math/Vector3.h"
#include <d3d12.h>
#include <wrl.h>

namespace GameEngine {
	class DirectionalLight {
	public:

		struct DirectionalLightData {
			Vector4 color; // ライトの色
			Vector3 direction; // ライトの向き
			float intensity; // 輝度
		};

	public:
		DirectionalLight() = default;
		~DirectionalLight() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		inline ID3D12Resource* GetResource() const { return directionalLightResource_.Get(); }

		/// <summary>
		/// ライト方向をセット
		/// </summary>
		/// <param name="lightdir">ライト方向</param>
		void SetLightDir(const Vector3& lightdir);

		/// <summary>
		/// ライト方向を取得
		/// </summary>
		/// <returns>ライト方向</returns>
		inline const Vector3& GetLightDir() const { return directionalLightData_->direction; }

		/// <summary>
		/// ライト色をセット
		/// </summary>
		/// <param name="lightcolor">ライト色</param>
		inline void SetLightColor(const Vector4& lightcolor) { directionalLightData_->color = lightcolor; }

		/// <summary>
		/// ライト色を取得
		/// </summary>
		/// <returns>ライト色</returns>
		inline const Vector4& GetLightColor() const { return directionalLightData_->color; }

		/// <summary>
		/// ライトの明度をセット
		/// </summary>
		/// <param name="lightIntensity"></param>
		inline void SetLightIntensity(const float& lightIntensity) { directionalLightData_->intensity = lightIntensity; }

		/// <summary>
		/// ライトの明度を取得
		/// </summary>
		/// <returns></returns>
		inline const float GetLightIntensity() const { return directionalLightData_->intensity; }

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

		// 平行光源のデータを作る
		DirectionalLightData* directionalLightData_ = nullptr;
	};
}