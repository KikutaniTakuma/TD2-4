#include "Vector4.h"
#include "Mat4x4.h"
#include "Vector3.h"
#include "Vector2.h"
#include <cmath>
#include <limits>

const Vector4 Vector4::kIdentity = { 1.0f,1.0f,1.0f,1.0f };
const Vector4 Vector4::kZero = { 0.0f, 0.0f, 0.0f, 0.0f };
const Vector4 Vector4::kXIdentity= { 1.0f, 0.0f, 0.0f, 0.0f };
const Vector4 Vector4::kYIdentity= { 0.0f, 1.0f, 0.0f, 0.0f };
const Vector4 Vector4::kZIdentity= { 0.0f, 0.0f, 1.0f, 0.0f };
const Vector4 Vector4::kWIdentity= { 0.0f, 0.0f, 0.0f, 1.0f };

Vector4::Vector4() noexcept :
	m{0.0f}
{}

Vector4::Vector4(const Vector4& right) noexcept 
{
	*this = right;
}
Vector4::Vector4(Vector4&& right)  noexcept :
	m(std::move(right.m))
{}

Vector4::Vector4(float x, float y, float z, float w) noexcept :
	m{x,y,z,w}
{}

Vector4::Vector4(const Vector3& vec3, float w) noexcept :
	m{vec3.x,vec3.y,vec3.z,w }
{}

Vector4::Vector4(const Vector2& vec2, float z, float w) noexcept :
	m{ vec2.x,vec2.y,z,w }
{}

Vector4::Vector4(uint32_t right) noexcept {
	*this = right;
}

Vector4& Vector4::operator=(const Vector4& right) noexcept {
	std::copy(right.m.begin(), right.m.end(), m.begin());

	return *this;
}

Vector4& Vector4::operator=(const Vector3& right) noexcept {
	vec.x = right.x;
	vec.y = right.y;
	vec.z = right.z;
	vec.w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(const Vector2& right) noexcept {
	vec.x = right.x;
	vec.y = right.y;
	vec.z = 0.0f;
	vec.w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(Vector4&& right) noexcept {
	m = std::move(right.m);

	return *this;
}

Vector4& Vector4::operator=(uint32_t right) noexcept {
	*this = UintToVector4(right);
	return *this;
}

Vector4 Vector4::operator+(const Vector4& right) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] + right.m[i];
	}

	return result;
}

Vector4& Vector4::operator+=(const Vector4& right) noexcept {
	*this = *this + right;

	return *this;
}

Vector4 Vector4::operator-(const Vector4& right) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] - right.m[i];
	}

	return result;
}

Vector4& Vector4::operator-=(const Vector4& right) noexcept {
	*this = *this - right;

	return *this;
}

Vector4 Vector4::operator*(float scalar) const noexcept {
	Vector4 result;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] * scalar;
	}

	return result;
}
Vector4& Vector4::operator*=(float scalar) noexcept {
	*this = *this * scalar;

	return *this;
}

Vector4 Vector4::operator/(float scalar) const noexcept {
	Vector4 result;
	scalar = 1.0f / scalar;

	for (size_t i = 0; i < result.m.size(); i++) {
		result.m[i] = m[i] * scalar;
	}

	return result;
}
Vector4& Vector4::operator/=(float scalar) noexcept {
	*this = *this / scalar;

	return *this;
}

Vector4 Vector4::operator*(const Mat4x4& mat) const noexcept {
	Vector4 result;

	auto tmp = mat;
	tmp.Transepose();

	for (int32_t i = 0; i < m.size(); i++) {
		result.m[i] = Dot(tmp[i]);
	}

	return result;
}

Vector4& Vector4::operator*=(const Mat4x4& mat) noexcept {
	*this = *this * mat;

	return *this;
}

Vector4 operator*(const Mat4x4& left, const Vector4& right) noexcept {
	Vector4 result;

	for (int32_t y = 0; y < 4; y++) {
		result.m[y] = left[y].Dot(right);
	}

	return result;
}

bool Vector4::operator==(const Vector4& right) const noexcept {
	return m == right.m;
}
bool Vector4::operator!=(const Vector4& right) const noexcept {
	return m != right.m;
}

float Vector4::Length() const noexcept {
	return std::sqrt(Dot(*this));
}

Vector4 Vector4::Normalize() const noexcept {
	if (*this == Vector4()) {
		return *this;
	}

	float nor = 1.0f / this->Length();

	return Vector4(*this) * nor;
}

float Vector4::Dot(const Vector4& right) const noexcept {
	return _mm_cvtss_f32(_mm_dp_ps(m128, right.m128, 0xff));
}

Vector3& Vector4::GetVector3() noexcept {
	return reinterpret_cast<Vector3&>(vec);
}
const Vector3& Vector4::GetVector3() const noexcept {
	return reinterpret_cast<const Vector3&>(vec);
}
const Vector2& Vector4::GetVector2() const noexcept {
	return reinterpret_cast<const Vector2 &>(vec);
}

uint32_t Vector4::GetColorRGBA() const
{
	uint32_t result = 0u;

	result += static_cast<uint32_t>(color.r * 255.0f) << 24;
	result += static_cast<uint32_t>(color.g * 255.0f) << 16;
	result += static_cast<uint32_t>(color.b * 255.0f) << 8;
	result += static_cast<uint32_t>(color.a * 255.0f);

	return result;
}

Vector4 UintToVector4(uint32_t color) {
	static constexpr float normal = 1.0f / static_cast<float>(std::numeric_limits<uint8_t>::max());
	Vector4 result;

	result.color = {
		static_cast<float>((color & 0xff000000) >> 24) * normal,
		static_cast<float>((color & 0x00ff0000) >> 16) * normal,
		static_cast<float>((color & 0x0000ff00) >>  8) * normal,
		static_cast<float>( color & 0x000000ff       ) * normal
	};
	return result;
}

uint32_t Vector4ToUint(const Vector4& color) {
	return color.GetColorRGBA();
}

Vector4 ColorLerp(const Vector4& start, const Vector4& end, float t) {
	Vector4 result;

	for (auto i = 0llu; i < result.m.size();i++) {
		result[i] = std::lerp(start[i], end[i], t);
	}

	return result;
}

uint32_t ColorLerp(uint32_t start, uint32_t end, float t) {
	Vector4 result;
	Vector4&& startTmp = UintToVector4(start);
	Vector4&& endTmp = UintToVector4(end);

	for (auto i = 0llu; i < result.m.size(); i++) {
		result[i] = std::lerp(startTmp[i], endTmp[i], t);
	}

	return Vector4ToUint(result);
}