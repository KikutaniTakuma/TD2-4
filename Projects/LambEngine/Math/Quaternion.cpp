#include "Quaternion.h"
#include "Mat4x4.h"
#include <cmath>
#include <numbers>

/// ========================================================================
/// 静的メンバ定数
/// ========================================================================
#pragma region Constant Number
const Quaternion Quaternion::kIdentity = { 0.0f, 0.0f, 0.0f, 1.0f };
const Quaternion Quaternion::kZero = { 0.0f, 0.0f, 0.0f, 0.0f };
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// コンストラクタ
/// ========================================================================
#pragma region Constructor
constexpr Quaternion::Quaternion():
	m({ 0.0f })
{}

Quaternion::Quaternion(const Vector4& right):
	Quaternion()
{
	*this = right;
}

Quaternion::Quaternion(const Vector3& right, float w) {
	vector.vector3 = right;
	vector.w = w;
}

Quaternion::Quaternion(const std::array<float, 4>& right) {
	m = right;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	m = { x,y,z,w };
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// コピー演算子
/// ========================================================================
#pragma region Copy operator
Quaternion& Quaternion::operator=(const Vector4& right) {
	m = right.m;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 単項演算子
/// ========================================================================
#pragma region Unary operator
Quaternion Quaternion::operator+() const noexcept {
	return *this;
}
Quaternion Quaternion::operator-() const noexcept {
	return Quaternion{ -m[0],-m[1],-m[2],-m[3] };
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 乗算演算子
/// ========================================================================
#pragma region Multiplication operator
Quaternion Quaternion::operator*(const Quaternion& right) const {
	Quaternion result;

	result = Quaternion{
		vector.vector3.Cross(right.vector.vector3) + vector.vector3 * right.vector.w + right.vector.vector3 * vector.w,
		vector.w * right.vector.w - vector.vector3.Dot(right.vector.vector3)
	};

	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& right) {
	*this = *this * right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 加算演算子
/// ========================================================================
#pragma region Add operator
Quaternion Quaternion::operator+(const Quaternion& right) const {
	Quaternion result;

	result = vector4 + right.vector4;

	return result;
}
Quaternion& Quaternion::operator+=(const Quaternion& right) {
	*this = *this + right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 減算演算子
/// ========================================================================
#pragma region Sub operator
Quaternion Quaternion::operator-(const Quaternion& right) const {
	Quaternion result;

	result = vector4 - right.vector4;

	return result;
}
Quaternion& Quaternion::operator-=(const Quaternion& right) {
	*this = *this - right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// スカラー倍演算子
/// ========================================================================
#pragma region Scalar operator
Quaternion Quaternion::operator*(float right) const {
	Quaternion result{ *this };

	result.vector4 *= right;

	return result;
}
Quaternion operator*(float right, const Quaternion& left) {
	return left * right;
}
Quaternion& Quaternion::operator*=(float right) {
	*this = *this * right;

	return *this;
}

Quaternion Quaternion::operator/(float right) const {
	return *this * (1.0f / right);
}
[[nodiscard]] Quaternion operator/(float right, const Quaternion& left) {
	return left / right;
}
Quaternion& Quaternion::operator/=(float right) {
	*this = *this / right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 等値演算子
/// ========================================================================
#pragma region Equal operator
bool Quaternion::operator==(const Quaternion& right) const {
	return m == right.m;
}
bool Quaternion::operator!=(const Quaternion& right) const {
	return m != right.m;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================


/// ========================================================================
/// メンバ関数
/// ========================================================================
#pragma region Member function
Quaternion Quaternion::Conjugate() const {
	return Quaternion{ -vector.vector3, vector.w };
}

float Quaternion::Dot(const Quaternion& other) const {
	return _mm_cvtss_f32(_mm_dp_ps(m128, other.m128, 0xff));
}

float Quaternion::Length() const {
	return std::sqrt(Dot(*this));
}

Quaternion Quaternion::Normalize() const {
	if (*this == Quaternion::kZero) {
		return *this;
	}

	return Quaternion{ *this } / this->Length();
}

Quaternion Quaternion::Inverce() const {
	return Conjugate() / std::pow(Length(), 2.0f);
}

Vector3 Quaternion::GetDirectionX() const {
	return Vector3(
		std::pow(quaternion.w, 2.0f) + std::pow(quaternion.x, 2.0f) - std::pow(quaternion.y, 2.0f) - std::pow(quaternion.z, 2.0f),
		2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z),
		2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y)
	);
}

Vector3 Quaternion::GetDirectionY() const
{
	return Vector3(
		2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z),
		std::pow(quaternion.w, 2.0f) - std::pow(quaternion.x, 2.0f) + std::pow(quaternion.y, 2.0f) - std::pow(quaternion.z, 2.0f),
		2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x)
	);
}

Vector3 Quaternion::GetDirectionZ() const
{
	return Vector3(
		2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y),
		2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x),
		std::pow(quaternion.w, 2.0f) - std::pow(quaternion.x, 2.0f) - std::pow(quaternion.y, 2.0f) + std::pow(quaternion.z, 2.0f)
	);
}

Mat4x4 Quaternion::GetMatrix() const {
	Vector3&& directionX = GetDirectionX();
	Vector3&& directionY = GetDirectionY();
	Vector3&& directionZ = GetDirectionZ();


	Mat4x4 result = Mat4x4{
			directionX.x,
			directionX.y,
			directionX.z,
			0.0f,

			directionY.x,
			directionY.y,
			directionY.z,
			0.0f,

			directionZ.x,
			directionZ.y,
			directionZ.z,
			0.0f,

			0.0f,0.0f,0.0f,1.0f
	};


	return result;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 静的メンバ関数
/// ========================================================================
#pragma region Static member function
Quaternion Quaternion::DirectionToDirection(const Vector3& from, const Vector3& to) {
	Quaternion result;
	Vector3 normal;
	float theata = std::acos(from.Dot(to)) * 0.5f;

	if (from.Dot(to) == -1.0f) {
		if (from.x != 0.0f || from.y != 0.0f) {
			normal = Vector3{ from.y, -from.x, 0.0f }.Normalize();
		}
		else if (from.x != 0.0f || from.z != 0.0f) {
			normal = Vector3{ from.z, 0.0f, -from.x }.Normalize();
		}
	}
	else {
		normal = from.Cross(to).Normalize();
	}


	result.vector.w = std::cos(theata);
	result.vector.vector3 = normal * std::sin(theata);

	return result;
}

Quaternion Quaternion::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Quaternion result;
	result.vector.w = std::cos(angle * 0.5f);
	result.vector.vector3 = axis * std::sin(angle * 0.5f);

	return result;
}
Quaternion Quaternion::MakeRotateXAxis(float angle) {
	return Quaternion{ std::sin(angle * 0.5f),0.0f,0.0f,std::cos(angle * 0.5f), };
}
Quaternion Quaternion::MakeRotateYAxis(float angle) {
	return Quaternion{ 0.0f, std::sin(angle * 0.5f),0.0f,std::cos(angle * 0.5f) };
}
Quaternion Quaternion::MakeRotateZAxis(float angle) {
	return Quaternion{ 0.0f, 0.0f, std::sin(angle * 0.5f),std::cos(angle * 0.5f) };
}

Quaternion Quaternion::EulerToQuaternion(const Vector3& euler)
{
	float cz = std::cos(euler.z * 0.5f);
	float sz = std::sin(euler.z * 0.5f);
	float cy = std::cos(euler.y * 0.5f);
	float sy = std::sin(euler.y * 0.5f);
	float cx = std::cos(euler.x * 0.5f);
	float sx = std::sin(euler.x * 0.5f);

	Quaternion result;
	result.quaternion.w = cz * cy * cx + sz * sy * sx;
	result.quaternion.x = cz * cy * sx - sz * sy * cx;
	result.quaternion.y = sz * cy * sx + cz * sy * cx;
	result.quaternion.z = sz * cy * cx - cz * sy * sx;

	return result.Normalize();
}

Quaternion Quaternion::Slerp(Quaternion start, const Quaternion& end, float t) {
	float dot = start.Dot(end);
	if (dot < 0.0f) {
		start = -start;
		dot = -dot;
	}
	float theata = std::acos(dot);
	float sinTheata = 1.0f / std::sin(theata);

	static constexpr float kEpsilon = 0.0005f;

	Quaternion result;

	// sinθが0.0fになる場合またはそれに近くなる場合
	if (1.0f - kEpsilon <= dot) {
		result = (1.0f - t) * start + t * end;
	}
	else {
		result = (std::sin(theata * (1.0f - t)) * sinTheata) * start + (std::sin(theata * t) * sinTheata) * end;
	}

	return result;
}
LogQuaternion Quaternion::Log(const Quaternion& quaternion)
{
	LogQuaternion result;
	float theata = std::acos(quaternion.quaternion.w);
	if (theata == 0.0f) {
		return result;
	}
	Vector3&& u = quaternion.vector.vector3 / std::sin(theata);
	result = u * theata;
	
	return result;
}
Quaternion Quaternion::Exp(const LogQuaternion& logQuaternion)
{
	Quaternion result;
	float theata = logQuaternion.Length();
	result.vector.w = std::cos(theata);
	if (theata == 0.0f) {
		return result;
	}
	Vector3&& u = logQuaternion / theata;
	result.vector.vector3 = u * std::sin(theata);

	return result;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================