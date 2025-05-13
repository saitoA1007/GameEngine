#pragma once
#include <d3d12.h>
#include"VertexData.h"
#include"EngineSource/Math/Vector4.h"
#include"EngineSource/Math/Vector3.h"
#include"EngineSource/Math/Vector2.h"
#include"EngineSource/Math/Matrix4x4.h"
#include"EngineSource/Math/TransformationMatrix.h"
#include"EngineSource/Common/LogManager.h"
#include<iostream>
#include<vector>
#include <wrl.h>

#include"Camera.h"
#include"Material.h"

namespace GameEngine {

	// テクスチャの前方宣言
	class TextureManager;
	class WorldTransform;

	class Model {
	public:

		struct MaterialData {
			std::string textureFilePath;
			Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
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
		/// <param name="objFilename">.objファイル名</param>
		/// <param name="filename">格納ファイル名</param>
		/// <returns></returns>
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
		/// <param name="material">マテリアル : 何の書かなければデフォルトのマテリアルを適応</param>
		void Draw(WorldTransform& worldTransform, const uint32_t& textureHandle, const Matrix4x4& VPMatrix,const Material* material = nullptr);

		/// <summary>
		/// モデルに光源を適応させる
		/// </summary>
		/// <param name="directionalLightResource"></param>
		void DrawLight(ID3D12Resource* directionalLightResource);

		/// <summary>
		/// デフォルトの色を設定
		/// </summary>
		/// <param name="color"></param>
		void SetDefaultColor(const Vector4& color);

		/// <summary>
		/// デフォオルトの光源の有無を設定
		/// </summary>
		/// <param name="isEnableLight"></param>
		void SetDefaultIsEnableLight(const bool& isEnableLight);

		/// <summary>
		/// デフォルトのuvMatrixを設定
		/// </summary>
		/// <param name="uvMatrix"></param>
		void SetDefaultUVMatrix(const Matrix4x4& uvMatrix);

	private:

		// OBJファイル読み込み
		ModelData LoadObjeFile(const std::string& directoryPath, const std::string& objFilename, const std::string& filename);

		// mtlファイルを読み込み
		MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

		// OBJファイルの面の成分を作成
		VertexData ParseVertex(
			const std::string& vertexDefinition,
			const std::vector<Vector4>& positions,
			const std::vector<Vector2>& texcoords,
			const std::vector<Vector3>& normals
		);

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

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

		uint32_t totalVertices_ = 0;
		uint32_t totalIndices_ = 0;

		// デフォルトのマテリアル
		std::unique_ptr<Material> defaultMaterial_ = nullptr;
	};
}