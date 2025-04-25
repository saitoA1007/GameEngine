#include"DirectionalLight.h"
#include"Source/Common/CreateBufferResource.h"
#include"Source/Math/MyMath.h"
using namespace GameEngine;

void DirectionalLight::Initialize(ID3D12Device* device) {
	// 平行光源のリソースを作る。
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLightData));
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// デフォルト値を設定
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };// 色
	directionalLightData_->direction = { 0.0f,-1.0f,0.f };// 方向
	directionalLightData_->intensity = 1.0f;// 輝度
}

void DirectionalLight::SetLightDir(const Vector3& lightdir) {
	directionalLightData_->direction = Normalize(lightdir);
}