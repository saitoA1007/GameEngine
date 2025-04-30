#pragma once
#include"Camera.h"
#include"WorldTransform.h"
#include"Model.h"


namespace GameEngine {

	class AxisIndicator {
	public:
		AxisIndicator() = default;
		~AxisIndicator() = default;

		/// /// <summary>
		/// 初期化
		/// </summary>
		void Initialize(const std::string& modelName, ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Matrix4x4& rotate);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(const uint32_t& textureHandle);

	private:
		AxisIndicator(const AxisIndicator&) = delete;
		AxisIndicator& operator=(const AxisIndicator&) = delete;

		// モデル
		std::unique_ptr<Model> model_;
		// カメラ
		Camera camera_;
		// ワールドトランスフォーム
		WorldTransform worldTransform_;

		Transform transform_;

		// ビューポート
		D3D12_VIEWPORT viewport_{};
		// シザー矩形
		D3D12_RECT scissorRect_{};

		ID3D12GraphicsCommandList* commandList_;
	};
}