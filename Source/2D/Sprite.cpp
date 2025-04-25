#include"Sprite.h"
#include"Source/Common/CreateBufferResource.h"
#include"Source/Math/MyMath.h"
using namespace GameEngine;

ID3D12Device* Sprite::device_ = nullptr;
ID3D12GraphicsCommandList* Sprite::commandList_ = nullptr;
Matrix4x4 Sprite::orthoMatrix_;

void Sprite::StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height) {
	device_ = device;
	commandList_ = commandList;
	orthoMatrix_ = Multiply(MakeIdentity4x4(), MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 100.0f));
}

Sprite* Sprite::Create(Vector2 position, Vector2 size,  Vector4 color) {

	Sprite* sprite = new Sprite();

	sprite->position_ = position;
	sprite->size_ = size;

	// Sprite用の頂点リソースを作る
	sprite->vertexResourceSprite_ = CreateBufferResource(device_, sizeof(VertexData) * 4);
	
	// リソースの先頭のアドレスから使う
	sprite->vertexBufferViewSprite_.BufferLocation = sprite->vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	sprite->vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	sprite->vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);
	// 書き込むためのアドレスを取得
	sprite->vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->vertexDataSprite_));

	// 画像のサイズを決める
	float left = 0.0f;
	float right = size.x;
	float top = 0.0f;
	float bottom = size.y;
	
	// 頂点インデックス
	sprite->vertexDataSprite_[0].position = { left,bottom,0.0f,1.0f }; // 左下
	sprite->vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	sprite->vertexDataSprite_[0].normal = { 0.0f,0.0f,-1.0f };
	sprite->vertexDataSprite_[1].position = { left,top,0.0f,1.0f }; // 左上
	sprite->vertexDataSprite_[1].texcoord = { 0.0f,0.0f };
	sprite->vertexDataSprite_[1].normal = { 0.0f,0.0f,-1.0f };
	sprite->vertexDataSprite_[2].position = { right,bottom,0.0f,1.0f }; // 右下
	sprite->vertexDataSprite_[2].texcoord = { 1.0f,1.0f };
	sprite->vertexDataSprite_[2].normal = { 0.0f,0.0f,-1.0f };
	sprite->vertexDataSprite_[3].position = { right,top,0.0f,1.0f }; // 左上
	sprite->vertexDataSprite_[3].texcoord = { 1.0f,0.0f };
	sprite->vertexDataSprite_[3].normal = { 0.0f,0.0f,-1.0f };

	// Sprite用の頂点インデックスのリソースを作る
	sprite->indexResourceSprite_ = CreateBufferResource(device_, sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	sprite->indexBufferViewSprite_.BufferLocation = sprite->indexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	sprite->indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	sprite->indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;
	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	sprite->indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	// 三角形
	indexDataSprite[0] = 0;  indexDataSprite[1] = 1;  indexDataSprite[2] = 2;
	// 三角形2
	indexDataSprite[3] = 1;  indexDataSprite[4] = 3;  indexDataSprite[5] = 2;

	// Sprite用のマテリアルリソースを作る
	sprite->materialResourceSprite_ = CreateBufferResource(device_, sizeof(Material));
	// 書き込むためのアドレスを取得
	sprite->materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->materialDataSprite_));
	// 色の設定
	sprite->materialDataSprite_->color = color;
	// SpriteはLightingしないのでfalseに設定する
	sprite->materialDataSprite_->enableLighting = false;
	// UVTransform行列を初期化
	sprite->materialDataSprite_->uvTransform = MakeIdentity4x4();

	// Sprite用のTransformationMatrix用のリソースを作る。TransformationMatrix 1つ分のサイズを用意する
	sprite->transformationMatirxResourceSprite_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	sprite->transformationMatirxResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->transformationMatrixDataSprite_));
	// 座標を元にワールド行列の生成
	sprite->worldMatrix_ = MakeTranslateMatrix({ position.x,position.y,0.0f });
	// 座標を適用
	sprite->transformationMatrixDataSprite_->WVP = Multiply(sprite->worldMatrix_,orthoMatrix_);
	// 単位行列を書き込んでおく
	sprite->transformationMatrixDataSprite_->World = MakeIdentity4x4();

	return sprite;
}

void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE* textureSrvHandlesGPU) {
	// Spriteの描画。
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	commandList_->IASetIndexBuffer(&indexBufferViewSprite_);// IBVを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResourceSprite_->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, transformationMatirxResourceSprite_->GetGPUVirtualAddress());
	// SpriteがuvCheckerを描画するようにする
	commandList_->SetGraphicsRootDescriptorTable(2, *textureSrvHandlesGPU);
	// 描画！(DrawCall/ドローコール) 6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;
	// 座標を元にワールド行列の生成
	worldMatrix_ = MakeTranslateMatrix({ position.x,position.y,0.0f });
	// 座標を適用 
	transformationMatrixDataSprite_->WVP = Multiply(worldMatrix_, orthoMatrix_);
}

void Sprite::SetSize(const Vector2& size) {
	size_ = size;
	// 画像のサイズを決める
	float left = 0.0f;
	float right = size.x;
	float top = 0.0f;
	float bottom = size.y;

	// 頂点インデックス
	vertexDataSprite_[0].position = { left,bottom,0.0f,1.0f }; // 左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[0].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[1].position = { left,top,0.0f,1.0f }; // 左上
	vertexDataSprite_[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[1].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[2].position = { right,bottom,0.0f,1.0f }; // 右下
	vertexDataSprite_[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite_[2].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[3].position = { right,top,0.0f,1.0f }; // 左上
	vertexDataSprite_[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite_[3].normal = { 0.0f,0.0f,-1.0f };
}

void Sprite::SetColor(const Vector4& color) {
	// 色の設定
	materialDataSprite_->color = color;
}

void Sprite::SetUvMatrix(const Transform& transform) {
	// uv行列の設定
	materialDataSprite_->uvTransform = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
}