#include "Mat4x4.h"
#include "Quaternion.h"
#include <cmath>
#include "Math/MathCommon.h"

const Mat4x4 Mat4x4::kIdentity = DirectX::XMMatrixIdentity();

const Mat4x4 Mat4x4::kZero = Mat4x4();

constexpr Mat4x4::Matrix(const DirectX::XMMATRIX& xmMatrix) {
	xmMatrix_ = xmMatrix;
}

Mat4x4 Mat4x4::MakeTranslate(const Vector3& vec) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixTranslation(vec.x, vec.y, vec.z);

	return result;
}

Mat4x4 Mat4x4::MakeScalar(const Vector3& vec) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixScaling(vec.x, vec.y, vec.z);

	return result;
}

Mat4x4 Mat4x4::MakeRotateX(float rad) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixRotationX(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateY(float rad) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixRotationY(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotateZ(float rad) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixRotationZ(rad);

	return result;
}

Mat4x4 Mat4x4::MakeRotate(const Vector3& rad) {
	return DirectX::XMMatrixRotationRollPitchYawFromVector({ rad.x,rad.y, rad.z });
}

Mat4x4 Mat4x4::MakeRotate(const Quaternion& rad)
{
	return DirectX::XMMatrixRotationQuaternion(rad.m128);
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixAffineTransformation({ scale.x, scale.y, scale.z }, {}, Quaternion::EulerToQuaternion(rad).m128, { translate.x,translate.y,translate.z });


	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Quaternion& rad, const Vector3& translate)
{
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixAffineTransformation({ scale.x, scale.y, scale.z }, {}, rad.m128, { translate.x,translate.y,translate.z });

	return result;
}

Mat4x4 Mat4x4::MakeAffin(const Vector3& scale, const Vector3& from, const Vector3& to, const Vector3& translate) {
	Mat4x4 result;

	result.xmMatrix_ = DirectX::XMMatrixAffineTransformation({ scale.x, scale.y, scale.z }, {}, Quaternion::DirectionToDirection(from, to).m128, { translate.x,translate.y,translate.z });

	return result;
}

Mat4x4 Mat4x4::MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 result;

	/*
	result[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	result[1][1] = 1.0f / std::tan(fovY / 2.0f);
	result[2][2] = farClip / (farClip - nearClip);
	result[2][3] = 1.0f;
	result[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	*/

	result.xmMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearClip, farClip);

	return result;
}

Mat4x4 Mat4x4::MakeOrthographic(float width, float height, float nearClip, float farClip) {
	Mat4x4 result;

	/*
	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = 1.0f / (farClip - nearClip);
	result[3][3] = 1.0f;

	result[3][0] = (left + right) / (left - right);
	result[3][1] = (top + bottom) / (bottom - top);
	result[3][2] = nearClip / (nearClip - farClip);
	*/

	result.xmMatrix_ = DirectX::XMMatrixOrthographicLH(width, height, nearClip, farClip);

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
		vector_type{
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
	return DirectX::XMMatrixRotationAxis({ axis.x,axis.y,axis.z }, angle);
}


Vector3 Mat4x4::GetTranslate() const
{
	return Vector3(matrix_[3][0], matrix_[3][1], matrix_[3][2]);
}
Vector3 Mat4x4::GetScale() const {
	return Vector3(
		Lamb::Math::Length({ matrix_[0][0], matrix_[0][1], matrix_[0][2] }),
		Lamb::Math::Length({ matrix_[1][0], matrix_[1][1], matrix_[1][2] }),
		Lamb::Math::Length({ matrix_[2][0], matrix_[2][1], matrix_[2][2] })
	);
}
Quaternion Mat4x4::GetRotate() const {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
	DirectX::XMMatrixDecompose(
		reinterpret_cast<DirectX::XMVECTOR*>(scale.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(rotate.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(translate.data()),
		this->xmMatrix_
	);

	return rotate.Normalize();
}

void Mat4x4::Decompose(Vector3& scale, Quaternion& rotate, Vector3& translate) const {
	DirectX::XMMatrixDecompose(
		reinterpret_cast<DirectX::XMVECTOR*>(scale.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(rotate.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(translate.data()),
		this->xmMatrix_
	);
}

void Mat4x4::Decompose(Vector3& scale, Vector3& rotate, Vector3& translate) const {
	Quaternion quaternion;
	DirectX::XMMatrixDecompose(
		reinterpret_cast<DirectX::XMVECTOR*>(scale.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(quaternion.data()),
		reinterpret_cast<DirectX::XMVECTOR*>(translate.data()),
		this->xmMatrix_
	);

	rotate = Vector3::QuaternionToEuler(quaternion);
}