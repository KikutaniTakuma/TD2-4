#include "Collider.h"
#include "imgui.h"
#include <algorithm>

Collider::Collider() :
	scale_(Vector3::kIdentity),
	collisionPos_(Vector3::kZero),
	max_(Vector3::kIdentity * 0.5f),
	min_(Vector3::kIdentity * -0.5f)
{
	color_ = Vector4ToUint(Vector4::kIdentity);

	for (auto& i : lines_) {
		i = std::make_unique<Line>();
	}
}

void Collider::UpdateCollision() {
	max_ = Vector3::kIdentity * 0.5f;
	min_ = Vector3::kIdentity * -0.5f;

	max_ *= Mat4x4::MakeAffin(scale_, Vector3::kZero, collisionPos_);
	min_ *= Mat4x4::MakeAffin(scale_, Vector3::kZero, collisionPos_);
}


bool Collider::IsCollision(const Vector3& pos) {
	if (min_.x <= pos.x && pos.x <= max_.x) {
		if (min_.y <= pos.y && pos.y <= max_.y) {
			if (min_.z <= pos.z && pos.z <= max_.z) {
				color_ = Vector4ToUint(Vector4::kXIdentity);
				return true;
			}
		}
	}
	color_ = Vector4ToUint(Vector4::kIdentity);
	return false;
}

bool Collider::CollisionExtrusion(Collider& other) {
	if (other.scale_.Length() > scale_.Length()) {
		std::array<Vector3, 8> otherPositions = {
			Vector3(min_), // 左下手前
			Vector3(min_.x, min_.y, max_.z), // 左下奥
			Vector3(max_.x, min_.y, min_.z), // 右下手前
			Vector3(max_.x, min_.y, max_.z), // 右下奥

			Vector3(min_.x, max_.y, min_.z), // 左上手前
			Vector3(min_.x, max_.y, max_.z), // 左上奥
			Vector3(max_.x, max_.y, min_.z), // 右上手前
			Vector3(max_) // 右上奥
		};

		Vector3 closestPoint = {
			std::clamp(collisionPos_.x, other.min_.x, other.max_.x),
			std::clamp(collisionPos_.y, other.min_.y, other.max_.y),
			std::clamp(collisionPos_.z, other.min_.z, other.max_.z)
		};

		for (auto& otherPos : otherPositions) {
			if (other.IsCollision(otherPos)) {
				Vector3 otherToVec = other.collisionPos_ - collisionPos_;
				Vector3 direction = Vector3::kIdentity;
				if (direction.Dot(otherToVec) >= 0.0f
					) {
					Vector3 pushBack = closestPoint - otherPos;
					std::list<float> pushBackValues;
					for (size_t i = 0; i < 3llu; i++) {
						pushBackValues.push_back(std::abs(pushBack[i]));
					}
					auto minElement = std::min_element(pushBackValues.begin(), pushBackValues.end());
					size_t minIndex = std::distance(pushBackValues.begin(), minElement);
					other.collisionPos_[minIndex] += *minElement;
				}
				else {
					Vector3 pushBack = closestPoint - otherPos;
					std::list<float> pushBackValues;
					for (size_t i = 0; i < 3llu; i++) {
						pushBackValues.push_back(std::abs(pushBack[i]));
					}
					auto minElement = std::min_element(pushBackValues.begin(), pushBackValues.end());
					size_t minIndex = std::distance(pushBackValues.begin(), minElement);
					other.collisionPos_[minIndex] -= *minElement;
				}


				//std::erase(pushBackValues, maxElement);


				flg_ = true;
				other.flg_ = true;
				color_ = Vector4ToUint(Vector4::kXIdentity);
				other.color_ = color_;
				return static_cast<bool>(flg_);
			}
			else {
				flg_ = false;
				other.flg_ = false;
				color_ = Vector4ToUint(Vector4::kIdentity);
				other.color_ = color_;
			}
		}


		return static_cast<bool>(flg_);
	}

	std::array<Vector3, 8> otherPositions = {
		Vector3(other.min_), // 左下手前
		Vector3(other.min_.x, other.min_.y, other.max_.z), // 左下奥
		Vector3(other.max_.x, other.min_.y, other.min_.z), // 右下手前
		Vector3(other.max_.x, other.min_.y, other.max_.z), // 右下奥

		Vector3(other.min_.x, other.max_.y, other.min_.z), // 左上手前
		Vector3(other.min_.x, other.max_.y, other.max_.z), // 左上奥
		Vector3(other.max_.x, other.max_.y, other.min_.z), // 右上手前
		Vector3(other.max_) // 右上奥
	};

	Vector3 closestPoint = {
		std::clamp(other.collisionPos_.x, min_.x, max_.x),
		std::clamp(other.collisionPos_.y, min_.y, max_.y),
		std::clamp(other.collisionPos_.z, min_.z, max_.z)
	};

	for (auto& otherPos : otherPositions) {
		if (IsCollision(otherPos)) {
			Vector3 otherToVec = other.collisionPos_ - collisionPos_;
			Vector3 direction = Vector3::kIdentity;
			if (direction.Dot(otherToVec) >= 0.0f
				) {
				Vector3 pushBack = closestPoint - otherPos;
				std::list<float> pushBackValues;
				for (size_t i = 0; i < 3llu; i++) {
					pushBackValues.push_back(std::abs(pushBack[i]));
				}
				auto minElement = std::min_element(pushBackValues.begin(), pushBackValues.end());
				size_t minIndex = std::distance(pushBackValues.begin(), minElement);
				other.collisionPos_[minIndex] += *minElement;
			}
			else {
				Vector3 pushBack = closestPoint - otherPos;
				std::list<float> pushBackValues;
				for (size_t i = 0; i < 3llu; i++) {
					pushBackValues.push_back(std::abs(pushBack[i]));
				}
				auto minElement = std::min_element(pushBackValues.begin(), pushBackValues.end());
				size_t minIndex = std::distance(pushBackValues.begin(), minElement);
				other.collisionPos_[minIndex] -= *minElement;
			}


			//std::erase(pushBackValues, maxElement);


			flg_ = true;
			other.flg_ = true;
			color_ = Vector4ToUint(Vector4::kXIdentity);
			other.color_ = color_;
			return static_cast<bool>(flg_);
		}
		else {
			flg_ = false;
			other.flg_ = false;
			color_ = Vector4ToUint(Vector4::kIdentity);
			other.color_ = color_;
		}
	}


	return static_cast<bool>(flg_);
}

bool Collider::CollisionPush(Collider& other) {
	return other.CollisionExtrusion(*this);
}

void Collider::DebugDraw(const Mat4x4& viewProjection) {
	std::array<Vector3, 8> positions = {
		Vector3(min_), // 左下手前
		Vector3(min_.x, min_.y, max_.z), // 左下奥
		Vector3(max_.x, min_.y, min_.z), // 右下手前
		Vector3(max_.x, min_.y, max_.z), // 右下奥

		Vector3(min_.x, max_.y, min_.z), // 左上手前
		Vector3(min_.x, max_.y, max_.z), // 左上奥
		Vector3(max_.x, max_.y, min_.z), // 右上手前
		Vector3(max_) // 右上奥
	};

	lines_[0]->start = positions[0];
	lines_[0]->end = positions[1];

	lines_[1]->start = positions[0];
	lines_[1]->end = positions[2];

	lines_[2]->start = positions[0];
	lines_[2]->end = positions[4];


	lines_[3]->start = positions[3];
	lines_[3]->end = positions[1];

	lines_[4]->start = positions[3];
	lines_[4]->end = positions[2];

	lines_[5]->start = positions[3];
	lines_[5]->end = positions[7];


	lines_[6]->start = positions[5];
	lines_[6]->end = positions[4];

	lines_[7]->start = positions[5];
	lines_[7]->end = positions[7];

	lines_[8]->start = positions[5];
	lines_[8]->end = positions[1];


	lines_[9]->start = positions[6];
	lines_[9]->end = positions[4];

	lines_[10]->start = positions[6];
	lines_[10]->end = positions[7];

	lines_[11]->start = positions[6];
	lines_[11]->end = positions[2];

	for (auto& line : lines_) {
		line->color = color_;
		line->Draw(viewProjection);
	}
}

void Collider::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());

	ImGui::DragFloat3("pos", &collisionPos_.x, 0.01f);
	ImGui::DragFloat3("scale", &scale_.x, 0.01f, 0.001f, std::numeric_limits<float>::max());

	ImGui::End();
#endif // _DEBUG
}

void Collider::SetType(uint32_t type) {
	types_ = std::bitset<32>(type);
}

bool Collider::Filter(const Collider& other) const {
	return types_ != (types_ & ~other.types_);
}

//void Collider::Adjusment(Collider& other, const Vector3& moveVec) {
//	std::array<Vector3, 8> otherPositions = {
//		Vector3(other.min_), // 左下手前
//		Vector3(other.min_.x, other.min_.y, other.max_.z), // 左下奥
//		Vector3(other.max_.x, other.min_.y, other.min_.z), // 右下手前
//		Vector3(other.max_.x, other.min_.y, other.max_.z), // 右下奥
//
//		Vector3(other.min_.x, other.max_.y, other.min_.z), // 左上手前
//		Vector3(other.min_.x, other.max_.y, other.max_.z), // 左上奥
//		Vector3(other.max_.x, other.max_.y, other.min_.z), // 右上手前
//		Vector3(other.max_) // 右上奥
//	};
//
//	std::array<Vector3, 8> positions = {
//		Vector3(min_), // 左下手前
//		Vector3(min_.x, min_.y, max_.z), // 左下奥
//		Vector3(max_.x, min_.y, min_.z), // 右下手前
//		Vector3(max_.x, min_.y, max_.z), // 右下奥
//
//		Vector3(min_.x, max_.y, min_.z), // 左上手前
//		Vector3(min_.x, max_.y, max_.z), // 左上奥
//		Vector3(max_.x, max_.y, min_.z), // 右上手前
//		Vector3(max_) // 右上奥
//	};
//
//	Vector3 collisionPos;
//
//	for (auto& pos : otherPositions) {
//		if (IsCollision(pos)) {
//			flg_.flg_ = true;
//			collisionPos = pos;
//			break;
//		}
//	}
//
//	Vector3 mostNear;
//
//	if (flg_.OnEnter()) {
//		for (auto& pos : positions) {
//			if ((collisionPos - pos).Length() < (collisionPos - mostNear).Length()) {
//				mostNear = pos;
//			}
//			else if(mostNear == Vector3::kZero) {
//				mostNear = pos;
//			}
//		}
//	}
//
//	if (moveVec.x < 0.0f) {
//		other.pos_.x += std::abs((mostNear - collisionPos).x);
//	}
//	else if (moveVec.x > 0.0f) {
//		other.pos_.x -= std::abs((mostNear - collisionPos).x);
//	}
//
//	if (moveVec.y < 0.0f) {
//		other.pos_.y += std::abs((mostNear - collisionPos).y);
//	}
//	else if (moveVec.y > 0.0f) {
//		other.pos_.y -= std::abs((mostNear - collisionPos).y);
//	}
//
//	if (moveVec.z < 0.0f) {
//		other.pos_.z += std::abs((mostNear - collisionPos).z);
//	}
//	else if (moveVec.z > 0.0f) {
//		other.pos_.z -= std::abs((mostNear - collisionPos).z);
//	}
//}