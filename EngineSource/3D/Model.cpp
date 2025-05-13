#include"Model.h"
#include<fstream>
#include<sstream>
#include<cassert>
#include"EngineSource/Core/TextureManager.h"
#include"EngineSource/Common/CreateBufferResource.h"
#include"EngineSource/Math/MyMath.h"
#include"WorldTransform.h"
using namespace GameEngine;

ID3D12Device* Model::device_ = nullptr;
ID3D12GraphicsCommandList* Model::commandList_ = nullptr;
LogManager* Model::logManager_ = nullptr;
TextureManager* Model::textureManager_ = nullptr;

void Model::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, TextureManager* textureManager,LogManager* logManager) {
	device_ = device;
	commandList_ = commandList;
	logManager_ = logManager;
	textureManager_ = textureManager;
}

Model* Model::CreateSphere(uint32_t subdivision) {

	Model* model = new Model();

	// 頂点数とインデックス数を計算
	model->totalVertices_ = (subdivision + 1) * (subdivision + 1);
	model->totalIndices_ = subdivision * subdivision * 6;

	// 頂点バッファを作成
	// vertexResourceを作成
	model->vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * model->totalVertices_);
	// リソースの先頭のアドレスから使う
	model->vertexBufferView_.BufferLocation = model->vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	model->vertexBufferView_.SizeInBytes = sizeof(VertexData) * model->totalVertices_;
	// 1頂点あたりのサイズ
	model->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点データを生成
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	model->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 緯度分割1つ分の角度
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(subdivision);
	// 経度分割1つ分の角度
	const float kLonEvery = 2.0f * static_cast<float>(M_PI) / static_cast<float>(subdivision);
	for (uint32_t latIndex = 0; latIndex <= subdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;
		float v = 1.0f - static_cast<float>(latIndex) / static_cast<float>(subdivision);
		for (uint32_t lonIndex = 0; lonIndex <= subdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			float u = static_cast<float>(lonIndex) / static_cast<float>(subdivision);
			uint32_t start = latIndex * (subdivision + 1) + lonIndex;

			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord = { u, v };
			vertexData[start].normal = { vertexData[start].position.x, vertexData[start].position.y, vertexData[start].position.z };
		}
	}

	// インデックスバッファを作成
	// 球用の頂点インデックスのリソースを作る
	model->indexResource_ = CreateBufferResource(device_, sizeof(uint32_t) * model->totalIndices_);
	// リソースの先頭のアドレスから使う
	model->indexBufferView_.BufferLocation = model->indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	model->indexBufferView_.SizeInBytes = sizeof(uint32_t) * model->totalIndices_;
	// インデックスはuint32_tとする
	model->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// インデックスデータを生成
	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	model->indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	uint32_t index = 0;
	for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
			uint32_t a = (latIndex * (subdivision + 1)) + lonIndex;
			uint32_t b = a + subdivision + 1;
			uint32_t c = a + 1;
			uint32_t d = b + 1;

			// 三角形1
			indexData[index] = a;
			index++;
			indexData[index] = b;
			index++;
			indexData[index] = d;
			index++;

			// 三角形2
			indexData[index] = a;
			index++;
			indexData[index] = d;
			index++;
			indexData[index] = c;
			index++;
		}
	}

	// マテリアルを作成
	model->defaultMaterial_ = std::make_unique<Material>();
	model->defaultMaterial_->Initialize({1.0f,1.0f,1.0f,1.0f},false);

	return model;
}

Model* Model::CreateTrianglePlane() {

	Model* model = new Model();

	// 頂点数とインデックス数を計算
	model->totalVertices_ = 3;
	model->totalIndices_ = 0;

	// 頂点バッファを作成
	// vertexResourceを作成
	model->vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * model->totalVertices_);
	// リソースの先頭のアドレスから使う
	model->vertexBufferView_.BufferLocation = model->vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	model->vertexBufferView_.SizeInBytes = sizeof(VertexData) * model->totalVertices_;
	// 1頂点あたりのサイズ
	model->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点データを生成
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	model->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].position.w = 1.0f;
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { vertexData[0].position.x, vertexData[0].position.y, vertexData[0].position.z };
	// 上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].position.w = 1.0f;
	vertexData[1].texcoord = { 0.5f,0.0f };
	vertexData[1].normal = { vertexData[1].position.x, vertexData[1].position.y, vertexData[1].position.z };
	// 右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].position.w = 1.0f;
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { vertexData[2].position.x, vertexData[2].position.y, vertexData[2].position.z };

	// マテリアルを作成
	model->defaultMaterial_ = std::make_unique<Material>();
	model->defaultMaterial_->Initialize({ 1.0f,1.0f,1.0f,1.0f }, false);

	return model;
}

Model* Model::CreateFromOBJ(const std::string& objFilename, const std::string& filename) {

	if (logManager_) {
		logManager_->Log("\nCreateFromOBJ : Start loading OBJ file: " + filename + objFilename);
	}

	Model* model = new Model();

	// データを読み込む処理
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Loading OBJ file data");
	}
	ModelData modelData = model->LoadObjeFile("Resources", objFilename, filename);

	// 描画する時に利用する頂点数
	model->totalVertices_ = UINT(modelData.vertices.size());
	model->totalIndices_ = 0;

	// 頂点リソースを作る
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Creating vertexResource");
	}
	model->vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	model->vertexBufferView_.BufferLocation = model->vertexResource_->GetGPUVirtualAddress();// リソースの先頭のアドレスから使う
	model->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());// 使用するリソースのサイズは頂点サイズ
	model->vertexBufferView_.StrideInBytes = sizeof(VertexData);// 1頂点あたりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	model->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));// 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());// 頂点データをリソースにコピー

	// マテリアルを作成
	model->defaultMaterial_ = std::make_unique<Material>();
	model->defaultMaterial_->Initialize(modelData.material.color, false);

	// OBJが無事に作成されたログを出す
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Success loaded OBJ file: " + filename + objFilename);
	}

	return model;
}

// 描画
void Model::Draw(WorldTransform& worldTransform, const uint32_t& textureHandle, const Matrix4x4& VPMatrix,const Material* material) {

	// カメラ座標に変換
	worldTransform.SetWVPMatrix(VPMatrix);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルが設定されていなければデフォルトのマテリアルを使う
	if (material == nullptr) {
		commandList_->SetGraphicsRootConstantBufferView(0, defaultMaterial_->GetMaterialResource()->GetGPUVirtualAddress());
	} else {
		commandList_->SetGraphicsRootConstantBufferView(0, material->GetMaterialResource()->GetGPUVirtualAddress());
	}
	commandList_->SetGraphicsRootConstantBufferView(1, worldTransform.GetTransformResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureSrvHandlesGPU(textureHandle));
	if (totalIndices_ != 0) {
		commandList_->DrawIndexedInstanced(totalIndices_, 1, 0, 0, 0);
	} else {
		commandList_->DrawInstanced(totalVertices_, 1, 0, 0);
	}
}

void Model::DrawLight(ID3D12Resource* directionalLightResource) {
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
}

Model::ModelData Model::LoadObjeFile(const std::string& directoryPath, const std::string& objFilename, const std::string& filename) {

	ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename + "/" + objFilename);
	assert(file.is_open());

	// 頂点情報を取得
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			// 位置Xを反転
			position.x *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			// Texture座標系を左下から左上へ
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			// 法線Xを反転
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {

			VertexData triangle[3];

			std::vector<std::string> vertexDefinitions;
			std::string vertexDef;
			// 面データを挿入
			while (s >> vertexDef) {
				vertexDefinitions.push_back(vertexDef);
			}

			// 各面に対応
			if (vertexDefinitions.size() >= 3) {
				for (uint32_t i = 1; i < vertexDefinitions.size() - 1; ++i) {
					triangle[0] = ParseVertex(vertexDefinitions[0], positions, texcoords, normals);
					triangle[1] = ParseVertex(vertexDefinitions[i], positions, texcoords, normals);
					triangle[2] = ParseVertex(vertexDefinitions[i + 1], positions, texcoords, normals);

					// 頂点を逆順で登録
					modelData.vertices.push_back(triangle[2]);
					modelData.vertices.push_back(triangle[1]);
					modelData.vertices.push_back(triangle[0]);
				}	
			}
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, filename + "/" + materialFilename);
		}
	}
	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルを読んだ1行を格納する
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		} else if (identifier == "Kd") {
			// 色のデータを取得
			s >> materialData.color.x >> materialData.color.y >> materialData.color.z;
			// アルファ値は1にしておく
			materialData.color.w = 1.0f;
		}
	}
	return materialData;
}

VertexData Model::ParseVertex(
	const std::string& vertexDefinition,
	const std::vector<Vector4>& positions,
	const std::vector<Vector2>& texcoords,
	const std::vector<Vector3>& normals
) {
	std::istringstream v(vertexDefinition);
	std::string indexStr;   // 読み込んだデータを格納する
	uint32_t indices[3] = { 0 };

	for (uint32_t i = 0; i < 3; ++i) {

		if (!std::getline(v, indexStr, '/')) {
			break;
		}

		// 空文字列でなければ代入
		if (!indexStr.empty()) {
			// 文字型をint型に変換して代入
			indices[i] = std::stoi(indexStr);
		}
	}

	// 配列データの先頭がobjでは0ではなく1なのでこちらの配列に合わせるために-1をする
	Vector4 position = positions[indices[0] - 1];
	Vector2 texcoord = texcoords[indices[1] - 1];
	Vector3 normal = normals[indices[2] - 1];
	return { position, texcoord, normal };
}

void  Model::SetDefaultColor(const Vector4& color) {
	defaultMaterial_->SetColor(color);
}

void  Model::SetDefaultIsEnableLight(const bool& isEnableLight) {
	defaultMaterial_->SetEnableLighting(isEnableLight);
}

void  Model::SetDefaultUVMatrix(const Matrix4x4& uvMatrix) {
	defaultMaterial_->SetUVMatrix(uvMatrix);
}