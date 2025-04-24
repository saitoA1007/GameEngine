#pragma once
#include"Source/Math/Vector4.h"
#include"Source/Math/Matrix4x4.h"
#include<iostream>

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
