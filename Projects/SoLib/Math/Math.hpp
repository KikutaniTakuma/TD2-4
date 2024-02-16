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

	}

}