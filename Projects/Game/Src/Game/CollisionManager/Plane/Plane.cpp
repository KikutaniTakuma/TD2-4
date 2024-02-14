#include "Plane.h"

bool IsCollisioPlaneSegment(const Plane& plane, const Segment& segment) {
	Vector3 b = segment.diff - segment.origin;

	float dot = b.Dot(plane.normal);

	if (dot == 0.0f) { return false; };

	float t = (plane.distance - segment.origin.Dot(plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}

	return false;
}

bool IsCollisionTriangle(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Segment& segment) {
	Vector3 normal = (pos1 - pos0).Cross(pos2 - pos1).Normalize();

	float distance = pos0.Dot(normal);

	Plane plane;
	plane.distance = distance;
	plane.normal = Project(normal, pos0).Normalize();

	Vector3 b = segment.diff;
	//b = b.Normalize();

	float dot = b.Dot(plane.normal);

	if (dot == 0.0f) { return false; };

	float t = (plane.distance - segment.origin.Dot(plane.normal)) / dot;
	if (!(0.0f >= t && t <= 1.0f)) {
		return false;
	}

	Vector3 p = b * t;
	p += segment.origin;

	Vector3 cross1 = (pos1 - pos0).Cross(p - pos1);
	Vector3 cross2 = (pos2 - pos1).Cross(p - pos2);
	Vector3 cross3 = (pos0 - pos2).Cross(p - pos0);


	if (cross1.Dot(normal) >= 0.0f &&
		cross2.Dot(normal) >= 0.0f &&
		cross3.Dot(normal) >= 0.0f
		) {
		return true;
	}

	return false;
}