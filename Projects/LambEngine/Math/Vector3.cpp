#include "Vector3.h"
#include <cmath>
#include "Mat4x4.h"
#include "Vector2.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Quaternion.h"

#include "Error/Error.h"

Vector3::Vector3() noexcept :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{}

Vector3::Vector3(float x, float y, float z) noexcept :
	x(x),
	y(y),
	z(z)
{}

Vector3::Vector3(const Vector3& right) noexcept
{
	*this = right;
}
Vector3::Vector3(const Vector2& right) noexcept {
	x = right.x;
	y = right.y;
	z = 0.0f;
}

Vector3::Vector3(Vector3&& right) noexcept
{
	*this = std::move(right);
}

const Vector3 Vector3::kIdentity = { 1.0f,1.0f,1.0f };
const Vector3 Vector3::kZero = { 0.0f, 0.0f,0.0f };
const Vector3 Vector3::kXIdentity = { 1.0f,0.0f,0.0f };
const Vector3 Vector3::kYIdentity = { 0.0f,1.0f,0.0f };
const Vector3 Vector3::kZIdentity = { 0.0f,0.0f,1.0f };

Vector3 Vector3::operator+() const noexcept {
	return *this;
}
Vector3 Vector3::operator-() const noexcept {
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator=(const Vector3& right) noexcept {
	x = right.x;
	y = right.y;
	z = right.z;

	return *this;
}

Vector3& Vector3::operator=(Vector3&& right) noexcept {
	x = std::move(right.x);
	y = std::move(right.y);
	z = std::move(right.z);

	return *this;
}

Vector3 Vector3::operator+(const Vector3& right) const noexcept {
	Vector3 tmp(x + right.x, y + right.y, z + right.z);

	return tmp;
}
Vector3 Vector3::operator-(const Vector3& right) const noexcept {
	Vector3 tmp(x - right.x, y - right.y, z - right.z);

	return tmp;
}
Vector3& Vector3::operator+=(const Vector3& right) noexcept {
	*this = *this + right;

	return *this;
}
Vector3& Vector3::operator-=(const Vector3& right) noexcept {
	*this = *this - right;

	return *this;
}

Vector3 Vector3::operator*(float scalar) const noexcept {
	Vector3 tmp(x * scalar, y * scalar, z * scalar);

	return tmp;
}
Vector3& Vector3::operator*=(float scalar) noexcept {
	*this = *this * scalar;

	return *this;
}

Vector3 Vector3::operator/(float scalar) const noexcept {
	Vector3 tmp(x / scalar, y / scalar, z / scalar);

	return tmp;
}
Vector3& Vector3::operator/=(float scalar) noexcept {
	*this = *this / scalar;

	return *this;
}

Vector3 Vector3::operator*(const Mat4x4& mat) const {
	Vector3 result;
	Vector4 vec = { *this,1.0f };
	result.x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + 1.0f * mat[3][0];
	result.y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + 1.0f * mat[3][1];
	result.z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + 1.0f * mat[3][2];
	float&& w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + 1.0f * mat[3][3];
	if (w == 0.0f) {
		throw Lamb::Error::Code<Vector3>("Vector3 * Matrix4x4 : w = 0.0f", __func__);
	}
	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3 operator*(const Mat4x4& left, const Vector3& right) {
	Vector3 result;
	Vector4 vec = { right,1.0f };

	result.x = left[0].Dot(vec);
	result.y = left[1].Dot(vec);
	result.z = left[2].Dot(vec);
	float&& w = left[3].Dot(vec);
	if (w == 0.0f) {
		throw Lamb::Error::Code<Vector3>("Matrix4x4 * Vector3 : w = 0.0f", __func__);
	}

	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3& Vector3::operator*=(const Mat4x4& mat) {
	*this = *this * mat;

	return *this;
}

Vector3& Vector3::operator=(const Vector2& right) noexcept {
	x = right.x;
	y = right.y;

	return *this;
}

Vector3 Vector3::operator*(const Quaternion& right) const {
	return (right * Quaternion{ *this, 0.0f } * right.Inverce()).vector.vector3;
}

Vector3& Vector3::operator*=(const Quaternion& right) {
	*this = *this * right;

	return *this;
}

bool Vector3::operator==(const Vector3& right) const noexcept {
	return x == right.x && y == right.y && z == right.z;
}
bool Vector3::operator!=(const Vector3& right) const noexcept {
	return x != right.x || y != right.y || z != right.z;
}

float& Vector3::operator[](size_t index) {
	if (3llu <= index) {
		throw Lamb::Error::Code<Vector3>("index is over", __func__);
	}
	std::array<float*,3> tmp = { &x,&y,&z };
	return *tmp[index];
}

const float& Vector3::operator[](size_t index) const {
	if (3llu <= index) {
		throw Lamb::Error::Code<Vector3>("index is over", __func__);
	}
	std::array<const float*, 3> tmp = { &x,&y,&z };
	return *tmp[index];
}

float Vector3::Length() const noexcept {
	return std::sqrt(x*x + y*y + z*z);
}

float Vector3::Dot(const Vector3& right) const noexcept {
	return x * right.x + y * right.y + z * right.z;
}

Vector3 Vector3::Cross(const Vector3& right) const noexcept {
	Vector3 result;

	result.x = y * right.z - z * right.y;
	result.y = z * right.x - x * right.z;
	result.z = x * right.y - y * right.x;

	return result;
}

Vector3 Vector3::Normalize() const noexcept {
	if (*this == Vector3::kZero) {
		return Vector3::kZero;
	}

	return *this / this->Length();
}


Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;

	result.x = std::lerp(start.x, end.x, t);
	result.y = std::lerp(start.y, end.y, t);
	result.z = std::lerp(start.z, end.z, t);

	return result;
}

Vector3 Project(const Vector3& vec1, const Vector3& vec2) {
	return vec2 * (vec1.Dot(vec2) / std::pow(vec2.Length(), 2.0f));
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	return segment.origin + Project(point - segment.origin, segment.diff);
}