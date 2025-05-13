#include"DirectionalLight.h"
#include"EngineSource/Common/CreateBufferResource.h"
#include"EngineSource/Math/MyMath.h"
using namespace GameEngine;

void DirectionalLight::Initialize(ID3D12Device* device,const Vector4& color,const Vector3& direction,const float& intensity) {
	// 平行光源のリソースを作る。
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLightData));
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// デフォルト値を設定
	directionalLightData_->color = color;// 色
	directionalLightData_->direction = direction;// 方向
	directionalLightData_->intensity = intensity;// 輝度
}

void DirectionalLight::SetLightDir(const Vector3& lightdir) {
	directionalLightData_->direction = Normalize(lightdir);
}