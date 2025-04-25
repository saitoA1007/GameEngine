#include"Model.h"
#include<fstream>
#include<sstream>
#include<cassert>
#include"Source/Core/TextureManager.h"
#include"Source/Common/CreateBufferResource.h"
#include"Source/Math/MyMath.h"

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

	// マテリアルリソースを作成
	// マテリアル用のリソースを作る。color1つ分のサイズを用意する
	model->materialResource_ = CreateBufferResource(device_, sizeof(Material));
	// 書き込むためのアドレスを取得
	model->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->materialData_));
	// 白色に設定
	model->materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Lightingするのでtureに設定する
	model->materialData_->enableLighting = true;
	// UVTransform行列を初期化
	model->materialData_->uvTransform = MakeIdentity4x4();

	// トランスフォーメーション行列リソースを作成
	// 球用のTransformationMatrix用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	model->transformationMatrixResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	model->transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->transformationMatrixData_));
	// 単位行列を書き込んでおく
	model->transformationMatrixData_->WVP = MakeIdentity4x4();
	model->transformationMatrixData_->World = MakeIdentity4x4();

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

	// マテリアル用のリソースを作る。color1つ分のサイズを用意する
	model->materialResource_ = CreateBufferResource(device_, sizeof(Material));
	// 書き込むためのアドレスを取得
	model->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->materialData_));
	// 白色に設定
	model->materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Lightingするのでtureに設定する
	model->materialData_->enableLighting = false;
	// UVTransform行列を初期化
	model->materialData_->uvTransform = MakeIdentity4x4();

	// トランスフォーメーション行列リソースを作成
	// 球用のTransformationMatrix用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	model->transformationMatrixResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	model->transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->transformationMatrixData_));
	// 単位行列を書き込んでおく
	model->transformationMatrixData_->WVP = MakeIdentity4x4();
	model->transformationMatrixData_->World = MakeIdentity4x4();

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

	// マテリアルリソースを作成
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Creating materialResource");
	}
	// マテリアル用のリソースを作る。color1つ分のサイズを用意する
	model->materialResource_ = CreateBufferResource(device_, sizeof(Material));
	// 書き込むためのアドレスを取得
	model->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->materialData_));
	// 白色に設定
	model->materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Lightingするのでtureに設定する
	model->materialData_->enableLighting = false;
	// UVTransform行列を初期化
	model->materialData_->uvTransform = MakeIdentity4x4();

	// トランスフォーメーション行列リソースを作成
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Creating transformationMatrixResource");
	}
	// 球用のTransformationMatrix用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	model->transformationMatrixResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	model->transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->transformationMatrixData_));
	// 単位行列を書き込んでおく
	model->transformationMatrixData_->WVP = MakeIdentity4x4();
	model->transformationMatrixData_->World = MakeIdentity4x4();

	// OBJが無事に作成されたログを出す
	if (logManager_) {
		logManager_->Log("CreateFromOBJ : Success loaded OBJ file: " + filename + objFilename);
	}

	return model;
}

// 描画
void Model::Draw(const Matrix4x4& worldMatrix, ID3D12Resource* directionalLightResource, const uint32_t& textureHandle, const Matrix4x4& VPMatrix) {

	// 変更した内容を適応
	transformationMatrixData_->WVP = Multiply(worldMatrix, VPMatrix);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureSrvHandlesGPU(textureHandle));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	if (totalIndices_ != 0) {
		commandList_->DrawIndexedInstanced(totalIndices_, 1, 0, 0, 0);
	} else {
		commandList_->DrawInstanced(totalVertices_, 1, 0, 0);
	}
}

Model::ModelData Model::LoadObjeFile(const std::string& directoryPath, const std::string& objFilename, const std::string& filename) {

	ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename + objFilename);
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
			normal.z *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へIndexは(位置/UV/法線)で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順で登録
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, filename + materialFilename);
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
		}
	}
	return materialData;
}

void Model::SetTransformationMatrix(const Matrix4x4& worldMatrix) {
	this->transformationMatrixData_->WVP = worldMatrix;
}

void Model::SetColor(const Vector4& color) {
	this->materialData_->color = color;
}

void Model::SetLightOn(const bool isLightOn) {
	this->materialData_->enableLighting = isLightOn;
}

void Model::SetUvMatrix(const Matrix4x4& uvMatrix) {
	this->materialData_->uvTransform = uvMatrix;
}