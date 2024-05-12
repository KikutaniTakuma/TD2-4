#include "Utils/Random/Random.h"

namespace Lamb {
	std::random_device seed;
	std::mt19937_64 rnd(seed());

	Vector2 Random(const Vector2& min, const Vector2& max) {
		Vector2 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);

		return result;
	}

	Vector3 Random(const Vector3& min, const Vector3& max) {
		Vector3 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);
		result.z = Random(min.z, max.z);

		return result;
	}

	uint32_t RandomColor(uint32_t min, uint32_t max) {
		return Vector4ToUint(RandomColor(Vector4{ min }, Vector4{ max }));
	}
	Vector4 RandomColor(const Vector4& min, const Vector4& max) {
		Vector4 result;
		result.vec.x = Random(min.vec.x, max.vec.x);
		result.vec.y = Random(min.vec.y, max.vec.y);
		result.vec.z = Random(min.vec.z, max.vec.z);
		result.vec.w = Random(min.vec.w, max.vec.w);

		return result;
	}
}