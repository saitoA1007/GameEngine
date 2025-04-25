#pragma once
#include <d3d12.h>
#include"VertexData.h"
#include"Material.h"
#include"Source/Math/Vector4.h"
#include"Source/Math/Vector3.h"
#include"Source/Math/Vector2.h"
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/TransformationMatrix.h"
#include"Source/Common/LogManager.h"
#include<iostream>
#include<vector>
#include <wrl.h>

#include"Camera.h"

namespace GameEngine {

	// テクスチャの前方宣言
	class TextureManager;

	class Model {
	public:

		struct MaterialData {
			std::string textureFilePath;
		};

		struct ModelData {
			std::vector<VertexData> vertices;
			MaterialData material;
		};

	public:
		Model() = default;
		~Model() = default;

		/// <summary>
		/// 静的初期化
		/// </summary>
		/// <param name="device"></param>
		/// <param name="commandList"></param>
		static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, TextureManager* textureManager, LogManager* logManager);

		/// <summary>
		/// OBJファイルからメッシュ生成
		/// </summary>
		/// <param name="modelname">モデル名</param>
		/// <returns>生成されたモデル</returns>
		static Model* CreateFromOBJ(const std::string& objFilename, const std::string& filename);

		/// <summary>
		/// 球モデル生成
		/// </summary>
		/// <returns>生成されたモデル</returns>
		static Model* CreateSphere(uint32_t subdivision);

		/// <summary>
		/// 三角形の平面を生成
		/// </summary>
		/// <returns></returns>
		static Model* CreateTrianglePlane();

		/// <summary>
		/// 生成したモデルを描画
		/// </summary>
		/// <param name="worldMatrix">ワールド行列</param>
		/// <param name="directionalLightResource">光源</param>
		/// <param name="textureHandle">テクスチャハンドル</param>
		/// <param name="VPMatrix"></param>
		void Draw(const Matrix4x4& worldMatrix, ID3D12Resource* directionalLightResource, const uint32_t& textureHandle, const Matrix4x4& VPMatrix);

		/// <summary>
		/// 3Dモデル移動処理
		/// </summary>
		/// <param name="worldMatrix"></param>
		void SetTransformationMatrix(const Matrix4x4& worldMatrix);

		/// <summary>
		/// 3Dモデルの色を変更
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// ライトを当てるか決める処理
		/// </summary>
		/// <param name="isLightOn">trueでライトが点く</param>
		void SetLightOn(const bool isLightOn);

		/// <summary>
		/// UV座標を変更する処理
		/// </summary>
		/// <param name="uvMatrix"></param>
		void SetUvMatrix(const Matrix4x4& uvMatrix);

	private:

		// OBJファイル読み込み
		ModelData LoadObjeFile(const std::string& directoryPath, const std::string& objFilename, const std::string& filename);

		// mtlファイルを読み込み
		MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	private:
		//Model() = default;
		Model(Model&) = delete;
		Model& operator=(Model&) = delete;

		// デバイス
		static ID3D12Device* device_;
		// コマンドリスト
		static ID3D12GraphicsCommandList* commandList_;

		// ログ
		static LogManager* logManager_;

		// テクスチャ
		static TextureManager* textureManager_;

		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

		uint32_t totalVertices_ = 0;
		uint32_t totalIndices_ = 0;

		TransformationMatrix* transformationMatrixData_ = nullptr;
		// マテリアルにデータを書き込む
		Material* materialData_ = nullptr;
	};
}