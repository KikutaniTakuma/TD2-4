#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"

struct Plane {
	Vector3 normal;
	float distance;
};

bool IsCollisioPlaneSegment(const Plane& plane, const Segment& segment);

bool IsCollisionTriangle(const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Segment& segment);