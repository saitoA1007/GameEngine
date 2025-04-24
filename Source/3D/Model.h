#pragma once
#include <d3d12.h>
#include"VertexData.h"
#include"Material.h"
#include"Source/Math/Vector4.h"
#include"Source/Math/Vector3.h"
#include"Source/Math/Vector2.h"
#include"Source/Math/Matrix4x4.h"
#include"Source/Math/TransformationMatrix.h"
#include<iostream>
#include<vector>
#include <wrl.h>

#include"Camera.h"

class Model {
public:

	struct MaterialData {
		std::string textureFilePath;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	enum ModelName {
		sphere,
		obj
	};

public:
	Model();
	~Model();

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

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

	// 描画
	void Draw(const Matrix4x4& worldMatrix, ID3D12Resource* directionalLightResource, D3D12_GPU_DESCRIPTOR_HANDLE* textureSrvHandlesGPU, const Matrix4x4& VPMatrix);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="worldMatrix"></param>
	void SetTransformationMatrix(const Matrix4x4& worldMatrix);

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

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	uint32_t totalVertices_ = 0;
	uint32_t totalIndices_ = 0;

	TransformationMatrix* transformationMatrixData_ = nullptr;

	// 生成したモデル名
	uint32_t modelname_ = 0;
};