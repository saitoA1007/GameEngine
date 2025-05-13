#pragma once
#include <iostream>
#include <random>
#include"EngineSource/Math/Vector3.h"

class RandomGenerator {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// int型のランダム値を取る
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	int GetInt(int min, int max);

	/// <summary>
	/// float型のランダム値を取る
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	float GetFloat(float min, float max);

	/// <summary>
	/// Vector3型のランダム値を取る
	/// </summary>
	/// <param name="min">s最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	Vector3 GetVector3(float min, float max);

private:
	std::mt19937 engine;
};