#pragma once

struct Vector2 {
	float x, y;

	Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
	Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
	Vector2 operator*(const Vector2& other) const { return { x * other.x, y * other.y }; }
	Vector2 operator/(const Vector2& other) const { return { x / other.x, y / other.y }; }
	Vector2 operator+=(const Vector2& other) { return { x += other.x, y += other.y }; }
	Vector2 operator-=(const Vector2& other) { return { x -= other.x, y -= other.y }; }
	Vector2 operator*=(const Vector2& other) { return { x *= other.x, y *= other.y }; }
	Vector2 operator/=(const Vector2& other) { return { x /= other.x, y /= other.y }; }
	Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
};
