#include"Math.hpp"
#include <cmath>

#include <assert.h>
#include <numbers>
#include <random>


Mat4x4 SoLib::Math::Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &transform) {
	return Mat4x4::MakeAffin(scale, rotate, transform);
}

Mat4x4 SoLib::Math::Affine(const Vector3 &scale, const Quaternion &quaternion, const Vector3 &transform) {
	Mat4x4 result;

	result = quaternion.GetMatrix();

	Vector4 *const matItr = reinterpret_cast<Vector4 *>(&result);
	for (uint8_t i = 0u; i < 3u; i++) {
		matItr[i] *= SoLib::begin(scale)[i];
	}
	*reinterpret_cast<Vector3 *>(&matItr[3]) = transform;

	return result;
}

Quaternion SoLib::Math::MakeQuaternion(const SoLib::Math::Euler &euler) {
	enum SinCos {
		kSin,
		kCos
	};

	const Vector3 hEuler = static_cast<const Vector3 &>(euler) / 2.f;

	const std::array<float, 2u> tX = { std::sin(hEuler.x), std::cos(hEuler.x) };
	const std::array<float, 2u> tY = { std::sin(hEuler.y), std::cos(hEuler.y) };
	const std::array<float, 2u> tZ = { std::sin(hEuler.z), std::cos(hEuler.z) };

	Quaternion result{
		/* x */ tX[kCos] * tY[kSin] * tZ[kSin] + tX[kSin] * tY[kCos] * tZ[kCos],
		/* y */ -tX[kSin] * tY[kCos] * tZ[kSin] + tX[kCos] * tY[kSin] * tZ[kCos],
		/* z */ tX[kCos] * tY[kCos] * tZ[kSin] + tX[kSin] * tY[kSin] * tZ[kCos],
		/* w */ -tX[kSin] * tY[kSin] * tZ[kSin] + tX[kCos] * tY[kCos] * tZ[kCos]
	};

	return result.Normalize();

}

Vector3 SoLib::Math::EulerToDirection(const SoLib::Math::Euler &euler) {
	Vector3 direction;
	enum SinCos {
		kSin,
		kCos
	};

	const std::array<float, 2u> eX = { std::sin(euler.x), std::cos(euler.x) };
	const std::array<float, 2u> eY = { std::sin(euler.y), std::cos(euler.y) };
	const std::array<float, 2u> eZ = { std::sin(euler.z), std::cos(euler.z) };

	direction.x = (eX[kCos] * eY[kSin] * eZ[kCos] + eX[kSin] * eZ[kSin]);
	direction.y = (eX[kCos] * eY[kSin] * eZ[kSin] - eX[kSin] * eZ[kCos]);
	direction.z = (eX[kCos] * eY[kCos]);
	return direction.Normalize();
}
