#include"Material.h"
#include"EngineSource/Math/MyMath.h"
#include"EngineSource/Common/CreateBufferResource.h"
using namespace GameEngine;

ID3D12Device* Material::device_ = nullptr;

void Material::StaticInitialize(ID3D12Device* device) {
	device_ = device;
}

void Material::Initialize(const Vector4& color,const bool& isEnableLighting) {
	// マテリアルリソースを作成
	// マテリアル用のリソースを作る。color1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(device_, sizeof(MaterialData));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 白色に設定
	materialData_->color = color;
	// Lightingするのでtureに設定する
	materialData_->enableLighting = isEnableLighting;
	// UVTransform行列を初期化
	materialData_->uvTransform = MakeIdentity4x4();
}
