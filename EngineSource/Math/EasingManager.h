#pragma once
#include"Vector3.h"

/// <summary>
/// 線形補間
/// </summary>
/// <param name="start">始点</param>
/// <param name="end">終点</param>
/// <param name="timer">時間</param>
/// <returns></returns>
float Lerp(const float& start, const float& end, const float& t);
Vector3 Lerp(const Vector3& start, const Vector3& end, const float& t);

// イージングイン
float EaseIn(const float& t);

// イージングアウト
float EaseOut(const float& t);

// イージングインアウト
float EaseInOut(const float& t);
