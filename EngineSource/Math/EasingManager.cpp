#include"EasingManager.h"
#include<cmath>

float Lerp(const float& start, const float& end, const float& t) {
	//return (1.0f - t) * start + t * end;
	return start + t * (end - start);
}

Vector3 Lerp(const Vector3& start, const Vector3& end, const float& t) {
	return Vector3(start.x + t * (end.x - start.x), start.y + t * (end.y - start.y) ,start.z + t * (end.z - start.z));
}

float EaseIn(const float& t) {
	return t * t;
}

float EaseOut(const float& t) {
	return 1.0f - std::powf(1.0f - t, 2.0f);
}

float EaseInOut(const float& t) {
	if (t < 0.5f) {
		return 2.0f * t * t;
	} else {
		return 1.0f - std::powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	}
}