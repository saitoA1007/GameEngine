#pragma once
#include <d3d12.h>
#include<iostream>
#include <wrl.h>
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/Vector2.h"
#include"Source/Math/Vector4.h"
#include"Source/Math/TransformationMatrix.h"
#include"Source/Math/Transform.h"

#include"Source/3D/VertexData.h"
#include"Source/3D/Material.h"

namespace GameEngine {

	class Sprite {
	public:
		/// <summary>
		/// 静的初期化
		/// </summary>
		/// <param name="device">デバイス</param>
		/// <param name="window_width">画面幅</param>
		/// <param name="window_height">画面高さ</param>
		static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height);

		/// <summary>
		/// スプライト生成
		/// </summary>
		/// <param name="texNumber">テクスチャハンドル</param>
		/// <param name="position">座標</param>
		/// <param name="color">色</param>
		/// <returns>生成されたスプライト</returns>
		static Sprite* Create(Vector2 position, Vector2 size, Vector4 color = { 1, 1, 1, 1 });

	public:

		/// <summary>
		/// 座標の設定
		/// </summary>
		/// <param name="position">座標</param>
		void SetPosition(const Vector2& position);

		const Vector2& GetPosition() const { return position_; }

		/// <summary>
		/// サイズの設定
		/// </summary>
		/// <param name="size">サイズ</param>
		void SetSize(const Vector2& size);

		const Vector2& GetSize() const { return size_; }

		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(const Vector4& color);

		const Vector4& GetColor() const { return materialDataSprite_->color; }

		/// <summary>
		/// uvMatrixの設定
		/// </summary>
		/// <param name="transform"></param>
		void SetUvMatrix(const Transform& transform);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(D3D12_GPU_DESCRIPTOR_HANDLE* textureSrvHandlesGPU);

	private:

		// デバイス
		static ID3D12Device* device_;
		// コマンドリスト
		static ID3D12GraphicsCommandList* commandList_;
		// 射影行列
		static Matrix4x4 orthoMatrix_;

		// 座標
		Vector2 position_{};
		// スプライト幅、高さ
		Vector2 size_ = { 100.0f, 100.0f };
		// ワールド行列
		Matrix4x4 worldMatrix_;

		// 頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};
		// Sprite用の頂点リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite_;
		VertexData* vertexDataSprite_ = nullptr;

		// 頂点バッファビューを作成する
		D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
		// Sprite用の頂点インデックスのリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite_;

		// Sprite用のマテリアルリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite_;
		// マテリアルにデータを書き込む
		Material::MaterialData* materialDataSprite_ = nullptr;

		// Sprite用のTransformationMatrix用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatirxResourceSprite_;
		TransformationMatrix* transformationMatrixDataSprite_ = nullptr;
	};
}
