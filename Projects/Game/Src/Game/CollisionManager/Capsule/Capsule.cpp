#include "Capsule.h"
#include <algorithm>

bool Lamb::Collision::Capsule(
	const Vector3& startPos, 
	const Vector3& endPos, 
	float capsuleRadius, 
	const Vector3& circlePos, 
	float circleRadius
) {
	Vector3 startToCircle = circlePos - startPos;
	Vector3 startToEnd = endPos - startPos;

	Vector3 normalStartToEnd = startToEnd.Normalize();

	float t = startToCircle.Dot(normalStartToEnd) / startToEnd.Length();

	t = std::clamp(t, 0.0f, 1.0f);

	Vector3 f = startPos * (1.0f - t) + endPos * t;

	return (circlePos - f).Length() < capsuleRadius + circleRadius;
}
