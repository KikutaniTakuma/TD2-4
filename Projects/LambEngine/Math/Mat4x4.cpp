#include "Mat4x4.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <cmath>

const Mat4x4 Mat4x4::kIdentity = BasedMatrix::Identity();

const Mat4x4 Mat4x4::kZero = Mat4x4();

Mat4x4::Mat4x4(const Mat4x4& right) {
	*this = right;
}

Mat4x4::Mat4x4(Mat4x4&& right) noexcept {
	*this = std::move(right);
}

Mat4x4::Mat4x4(const std::initializer_list<float>& right) {
	std::copy(right.begin(), right.end(), vector_.begin());
}

Mat4x4::Mat4x4(const BasedMatrix::VectorType& right) {
	std::copy(right.begin(), right.end(), vector_.begin());
}

Mat4x4::Mat4x4(const BasedMatrix& right) {
	*this = right;
}

Mat4x4& Mat4x4::operator=(const BasedMatrix& right) {
	std::copy(right.begin(), right.end(), this->begin());

	return *this;
}


Mat4x4 Mat4x4::MakeTranslate(const Vector3& vec) {
	Mat4x4 result;

	result[0][0] = 1.0f;
	result[1][1] = 1.0f;
	result[2][2] = 1.0f;
	result[3][3] = 1.0f;

	result[3][0] = vec.x;
	result[3][1] = vec.y;
	result[3][2] = vec.z;

	return result;
}

Mat4x4 Mat4x4::MakeScalar(const Vector3& vec) {
	Mat4x4 result;

	result[0][0] = vec.x;
	result[1][1] = vec.y;
	result[2][2] = vec.z;
	result[3][3] = 1.0f;

	return result;
}

Mat4x4 Mat4x4::MakeRotateX(float rad) {
	Mat4x4 result;

	result[0][0] = 1.0f;
	result[3][3] = 1.0f;

	result[1][1] = std::cos(rad);
	result[1][2] = std::sin(rad);
	result[2][1] = -std::sin(rad);
	result[2][2] = std::cos(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateY(float rad) {
	Mat4x4 result;

	result[1][1] = 1.0f;
	result[3][3] = 1.0f;

	result[0][0] = std::cos(rad);
	result[0][2] = -std::sin(rad);
	result[2][0] = std::sin(rad);
	result[2][2] = std::cos(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateZ(float rad) {
	Mat4x4 result;

	result[2][2] = 1.0f;
	result[3][3] = 1.0f;

	result[0][0] = std::cos(rad);
	result[0][1] = -std::sin(rad);
	result[1][0] = std::sin(rad);
	result[1][1] = std::cos(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotate(const Vector3& rad) {
	return  MakeAffin(Vector3::kIdentity, rad, Vector3::kZero);
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 result;

	Mat4x4 rotate = Mat4x4::MakeRotateX(rad.x) * Mat4x4::MakeRotateY(rad.y) * Mat4x4::MakeRotateZ(rad.z);

	result = Mat4x4{
		scale.x * rotate[0][0], scale.x * rotate[0][1],scale.x * rotate[0][2], 0.0f,
		scale.y * rotate[1][0], scale.y * rotate[1][1],scale.y * rotate[1][2], 0.0f,
		scale.z * rotate[2][0], scale.z * rotate[2][1],scale.z * rotate[2][2], 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};

	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Quaternion& rad, const Vector3& translate)
{
	Mat4x4 result;

	result = Mat4x4::MakeScalar(scale) * rad.GetMatrix() * Mat4x4::MakeTranslate(translate);

	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& from, const Vector3& to, const Vector3& translate) {
	Mat4x4 result;

	result = Mat4x4::MakeScalar(scale) * Mat4x4::DirectionToDirection(from, to) * Mat4x4::MakeTranslate(translate);

	return result;
}

Mat4x4 Mat4x4::MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 result;

	result[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	result[1][1] = 1.0f / std::tan(fovY / 2.0f);
	result[2][2] = farClip / (farClip - nearClip);
	result[2][3] = 1.0f;
	result[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

Mat4x4 Mat4x4::MakeOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Mat4x4 result;



	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = 1.0f / (farClip - nearClip);
	result[3][3] = 1.0f;

	result[3][0] = (left + right) / (left - right);
	result[3][1] = (top + bottom) / (bottom - top);
	result[3][2] = nearClip / (nearClip - farClip);

	return result;
}

Mat4x4 Mat4x4::MakeViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Mat4x4 result;

	result[0][0] = width / 2.0f;
	result[1][1] = height / -2.0f;
	result[2][2] = maxDepth - minDepth;
	result[3][3] = 1.0f;

	result[3][0] = left + (width / 2.0f);
	result[3][1] = top + (height / 2.0f);
	result[3][2] = minDepth;

	return result;
}

Mat4x4 Mat4x4::DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 normal;

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


	float theataCos = from.Dot(to);
	float theataSin = from.Cross(to).Length();

	Mat4x4 result = 
		BasedMatrix::VectorType{
			normal.x * normal.x * (1.0f - theataCos) + theataCos,
			normal.x * normal.y * (1.0f - theataCos) + normal.z * theataSin,
			normal.x * normal.z * (1.0f - theataCos) - normal.y * theataSin,
			0.0f,

			normal.y * normal.x * (1.0f - theataCos) - normal.z * theataSin,
			normal.y * normal.y * (1.0f - theataCos) + theataCos,
			normal.y * normal.z * (1.0f - theataCos) + normal.x * theataSin,
			0.0f,

			normal.z * normal.x * (1.0f - theataCos) + normal.y * theataSin,
			normal.z * normal.y * (1.0f - theataCos) - normal.x * theataSin,
			normal.z * normal.z * (1.0f - theataCos) + theataCos,
			0.0f,

			0.0f,0.0f,0.0f,1.0f
		};


	return result;
}

Mat4x4 Mat4x4::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	float angleCos = std::cos(angle);
	float angleSin = std::sin(angle);

	Mat4x4 result = BasedMatrix::VectorType{
			axis.x * axis.x * (1.0f - angleCos) + angleCos,
			axis.x * axis.y * (1.0f - angleCos) + axis.z * angleSin,
			axis.x * axis.z * (1.0f - angleCos) - axis.y * angleSin,
			0.0f,

			axis.y * axis.x * (1.0f - angleCos) - axis.z * angleSin,
			axis.y * axis.y * (1.0f - angleCos) + angleCos,
			axis.y * axis.z * (1.0f - angleCos) + axis.x * angleSin,
			0.0f,

			axis.z * axis.x * (1.0f - angleCos) + axis.y * angleSin,
			axis.z * axis.y * (1.0f - angleCos) - axis.x * angleSin,
			axis.z * axis.z * (1.0f - angleCos) + angleCos,
			0.0f,

			0.0f,0.0f,0.0f,1.0f
	};


	return result;
}