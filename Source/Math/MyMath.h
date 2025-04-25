#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"

static const double M_PI = 3.14159265358979323846;

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

// ベクトルの長さを求める
float Length(const Vector3& v);
// ベクトルの正規化
Vector3 Normalize(const Vector3& v);
// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// 外積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 単位行列を生成する関数
Matrix4x4 MakeIdentity4x4();

// 4x4行列の積
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

// 4xx4のX軸の回転行列を作成
Matrix4x4 MakeRotateXMatrix(const float& theta);

// 4x4のY軸の回転行列を作成
Matrix4x4 MakeRotateYMatrix(const float& theta);

// 4x4のZ軸の回転行列を作成
Matrix4x4 MakeRotateZMatrix(const float& theta);

// 4x4の拡縮行列の作成
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 4x4の平行移動行列の作成
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 4x4のSRTによるアフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& theta, const Vector3 translate);

// 4x4逆行列の計算
Matrix4x4 InverseMatrix(const Matrix4x4& matrix);

// 透視投影行列の作成
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 平行投射行列の作成
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// (3+1)次元座標系をデカルト座標系に変換
Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix);
