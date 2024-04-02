#include "AABB.h"
#include "../SoLib/Math/Math.hpp"

AABB AABB::AddPos(const Vector3 &vec) const {
	AABB result = *this;
	result.min += vec;
	result.max += vec;

	return result;
}

AABB AABB::Extend(const Vector3 &vec) const {

	AABB result = *this;
	result.SwapSelf();

	for (uint32_t i = 0u; i < 3u; ++i) {
		// もし正の数なら
		if ((&vec.x)[i] > 0.f) {
			// maxを加算
			(&result.max.x)[i] += (&vec.x)[i];
		}
		// もし負数なら
		else {
			// minに加算
			(&result.min.x)[i] += (&vec.x)[i];
		}
	}

	return result;
}

Vector3 AABB::GetCentor() const {

	return min + this->GetRadius();
}

Vector3 AABB::GetRadius() const {
	return (max - min) / 2.f;
}

const Vector3 &AABB::GetNormal(const Vector3 &surfacePoint) const {
	float resultSQ = 0.f;
	uint32_t resultIndex = 0u;
	// 各法線の軸
	static const std::array<const Vector3 *const, 3u> kNormalArray
	{	// 判定順序
		&Vector3::kYIdentity,
		&Vector3::kXIdentity,
		&Vector3::kZIdentity,
	};

	const Vector3 defaultNormal = (surfacePoint - this->GetCentor());
	// 各辺の長さを壱とした時の表面座標
	Vector3 scalingSurface;
	for (uint32_t i = 0; i < 3u; i++) {	// 半径のサイズで除算
		scalingSurface[i] = defaultNormal[i] / this->GetRadius()[i];
	}

	for (uint32_t i = 0; i < kNormalArray.size(); i++) {
		// 法線ベクトル
		const Vector3 &normal = *kNormalArray.at(i);

		// 法線ベクトルとの内積
		const Vector3 &dot = normal * normal.Dot(scalingSurface);
		// dotの絶対値が大きい要素を返す
		if (resultSQ < dot.LengthSQ()) {
			resultSQ = dot.LengthSQ();
			resultIndex = i;
		}
	}

	return *kNormalArray.at(resultIndex);
}

Mat4x4 AABB::MakeTransMat() const {
	return SoLib::Math::Affine(this->GetRadius(), Vector3::kZero, this->GetCentor());
}

AABB AABB::Create(const Vector3 &origin, const Vector3 &radius) {
	AABB result{};
	result.min = -radius;
	result.max = radius;
	result.SwapSelf();

	return result.AddPos(origin);
}

bool AABB::ImGuiDebug(const char *const group) {
	// 編集したか
	bool isChange = false;

	if (ImGui::TreeNode(group)) {

		isChange |= ImGui::DragFloat3("Max", &max.x, 0.1f);
		isChange |= ImGui::DragFloat3("Min", &min.x, 0.1f);
		// 編集したら実行
		if (isChange) {
			SwapSelf();
		}
		ImGui::TreePop();
	}
	return isChange;
}

const AABB &AABB::SwapSelf() {
	if (min.x > max.x) {
		std::swap(min.x, max.x);
	}
	if (min.y > max.y) {
		std::swap(min.y, max.y);
	}
	if (min.z > max.z) {
		std::swap(min.z, max.z);
	}
	return *this;
}

std::array<Vector3, 8u> AABB::GetVertex() const {
	std::array<Vector3, 8u> result{

		// lower
		Vector3{ min.x, min.y, min.z },
		Vector3{ min.x, min.y, max.z },
		Vector3{ max.x, min.y, max.z },
		Vector3{ max.x, min.y, min.z },
		// higher
		Vector3{ min.x, max.y, min.z },
		Vector3{ min.x, max.y, max.z },
		Vector3{ max.x, max.y, max.z },
		Vector3{ max.x, max.y, min.z },
	};
	return result;
}