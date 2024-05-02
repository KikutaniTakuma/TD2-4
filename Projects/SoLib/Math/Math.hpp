#pragma once

#include <numbers>

#include "Angle.h"
#include "Math/Quaternion.h"
#include "Math/Mat4x4.h"
#include "Euler.h"
//struct Polar;
//
//struct Vector2;
//struct Vector3;
//
//struct Matrix2x2;
//struct Matrix3x3;
//struct Matrix4x4;

//static const Vector2 TextSize{ 50, 20 };



namespace SoLib {

	namespace Math {

		Mat4x4 Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &transform);
		Mat4x4 Affine(const Vector3 &scale, const Quaternion &quaternion, const Vector3 &transform);


		Quaternion MakeQuaternion(const SoLib::Math::Euler &euler);

		Vector3 EulerToDirection(const SoLib::Math::Euler &euler);

		inline int32_t Sign(int32_t value) {
			return value == 0 ? 0 : value > 0 ? 1 : -1;	// 0なら0を返し、 0より大きかったら1を返す。マイナスなら-1を返す。
		}
		inline float Sign(float value) {
			return value == 0.f ? 0.f : value > 0.f ? 1.f : -1.f;	// 0なら0を返し、 0より大きかったら1を返す。マイナスなら-1を返す。
		}

	}

}