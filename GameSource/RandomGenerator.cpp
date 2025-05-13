#include"RandomGenerator.h"

void RandomGenerator::Initialize() {
	std::random_device rd;
	engine = std::mt19937(rd());
}

int RandomGenerator::GetInt(int min, int max) {
	std::uniform_int_distribution<> dist(min, max);
	return dist(engine);
}

float RandomGenerator::GetFloat(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(engine);
}

Vector3 RandomGenerator::GetVector3(float min, float max) {
	return Vector3(GetFloat(min, max), GetFloat(min, max), GetFloat(min, max));
}