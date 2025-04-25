#include"WorldTransform.h"
#include"Source/Math/MyMath.h"
using namespace GameEngine;


void WorldTransform::Initialize(const Transform& transform) {
	transform_ = transform;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}

void WorldTransform::TransformMatrix() {

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}