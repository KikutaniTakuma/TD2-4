#pragma once

#include <array>
#include <concepts>
#include <string>
#include <initializer_list>
#include "Matrix.h"

#include "Vector3.h"
#include "Quaternion.h"

/// <summary>
/// 4x4行列
/// </summary>
class Mat4x4 final : public Matrix<float, 4, 4> {
public:
	using BasedMatrix = Matrix<float, 4, 4>;

/// <summary>
/// コンストラクタ
/// </summary>
public:
	Mat4x4() = default;
	Mat4x4(const Mat4x4& right);
	Mat4x4(Mat4x4&& right) noexcept;
	Mat4x4(const std::initializer_list<float>& right);
	Mat4x4(const BasedMatrix::vector_type& right);
	Mat4x4(const BasedMatrix& right);

public:
	~Mat4x4() = default;

public:
	Mat4x4& operator=(const Mat4x4&) = default;
	Mat4x4& operator=(Mat4x4&&) = default;
	Mat4x4& operator=(const BasedMatrix& right);

	[[nodiscard]] bool operator==(const Mat4x4& right) const = default;
	[[nodiscard]] bool operator!=(const Mat4x4& right) const = default;

public:
	Vector3 GetTranslate();
	Vector3 GetScale();
	Quaternion GetRotate();

	void Decompose(Vector3& scale, Quaternion& rotate, Vector3& translate);

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	static [[nodiscard]] Mat4x4 MakeTranslate(const Vector3& vec);

	static [[nodiscard]] Mat4x4 MakeScalar(const Vector3& vec);

	static [[nodiscard]] Mat4x4 MakeRotateX(float rad);

	static [[nodiscard]] Mat4x4 MakeRotateY(float rad);

	static [[nodiscard]] Mat4x4 MakeRotateZ(float rad);

	static [[nodiscard]] Mat4x4 MakeRotate(const Vector3& rad);

	static [[nodiscard]] Mat4x4 MakeAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);
	
	static [[nodiscard]] Mat4x4 MakeAffin(const Vector3& scale, const class Quaternion& rad, const Vector3& translate);

	static [[nodiscard]] Mat4x4 MakeAffin(const Vector3& scale, const Vector3& from, const Vector3& to, const Vector3& translate);

	static [[nodiscard]] Mat4x4 MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	static [[nodiscard]] Mat4x4 MakeOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

	static [[nodiscard]] Mat4x4 MakeViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);


	static [[nodiscard]] Mat4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

	static [[nodiscard]] Mat4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

/// <summary>
/// 静的定数
/// </summary>
public:
	static const Mat4x4 kIdentity;
	static const Mat4x4 kZero;
};