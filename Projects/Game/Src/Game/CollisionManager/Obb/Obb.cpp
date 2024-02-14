#include "Obb.h"
#include <numbers>
#include <algorithm>
#include "imgui.h"
#include "Game/CollisionManager/Plane/Plane.h"

Obb::Obb():
	center_(),
	scale_(Vector3::kIdentity),
	rotate_(),
	worldMatrix_(),
	color_(std::numeric_limits<uint32_t>::max()),
	orientations_{
		Vector3::kXIdentity,
		Vector3::kYIdentity,
		Vector3::kZIdentity
	},
	lines_{},
	orientationLines_{}
{}

bool Obb::IsCollision(Vector3 pos, float radius) {
	std::array<Vector3, 3> orientations = {
		orientations_[0] * Mat4x4::MakeRotate(rotate_),
		orientations_[1] * Mat4x4::MakeRotate(rotate_),
		orientations_[2] * Mat4x4::MakeRotate(rotate_)
	};

	std::array<Vector3, 2> positions = {
		Vector3(-size_), // 左下手前
		
		Vector3(+size_) // 右上奥
	};

	for (auto& position : positions) {
		position *= Mat4x4::MakeScalar(scale_);
	}

	pos *= Mat4x4::MakeInverse(Mat4x4::MakeAffin(Vector3::kIdentity, rotate_, center_));

	//std::array<Plane, 6> planes = {
	//	Plane{-orientations[1].Normalize(), (center_ - orientations[1]).Length()}, //底面
	//	Plane{-orientations[0].Normalize(), (center_ - orientations[0]).Length()}, // 左面
	//	Plane{-orientations[2].Normalize(), (center_ - orientations[2]).Length()}, // 最前面
	//	Plane{+orientations[1].Normalize(), (center_ + orientations[1]).Length()}, //上面
	//	Plane{+orientations[0].Normalize(), (center_ + orientations[0]).Length()}, // 右面
	//	Plane{+orientations[2].Normalize(), (center_ + orientations[2]).Length()} // 最背面
	//};

	//for (auto& plane : planes) {

	//}

	Vector3 closestPoint = {
		std::clamp(pos.x, positions[0].x,positions[1].x),
		std::clamp(pos.y, positions[0].y,positions[1].y),
		std::clamp(pos.z, positions[0].z,positions[1].z)
	};

	float distance = (closestPoint - pos).Length();

	if (distance <= radius) {
		isCollision_ = true;
		color_ = 0xff0000ff;
		return true;
	}
	else {
		isCollision_ = false;
		color_ = std::numeric_limits<uint32_t>::max();
		return false;
	}
}

void Obb::Update() {
	worldMatrix_.Affin(scale_, rotate_, center_);

	size_ = scale_ * 0.5f;

	std::array<Vector3, 8> positions = {
		Vector3(-size_), // 左下手前
		Vector3(-size_.x, -size_.y, +size_.z), // 左下奥
		Vector3(+size_.x, -size_.y, -size_.z), // 右下手前
		Vector3(+size_.x, -size_.y, +size_.z), // 右下奥

		Vector3(-size_.x, +size_.y, -size_.z), // 左上手前
		Vector3(-size_.x, +size_.y, +size_.z), // 左上奥
		Vector3(+size_.x, +size_.y, -size_.z), // 右上手前
		Vector3(+size_) // 右上奥
	};

	lines_[0].start = positions[0];
	lines_[0].end = positions[1];

	lines_[1].start = positions[0];
	lines_[1].end = positions[2];

	lines_[2].start = positions[0];
	lines_[2].end = positions[4];

	lines_[3].start = positions[3];
	lines_[3].end = positions[1];

	lines_[3].start = positions[3];
	lines_[3].end = positions[1];

	lines_[4].start = positions[3];
	lines_[4].end = positions[2];

	lines_[5].start = positions[3];
	lines_[5].end = positions[7];

	lines_[6].start = positions[5];
	lines_[6].end = positions[4];

	lines_[6].start = positions[5];
	lines_[6].end = positions[4];

	lines_[7].start = positions[5];
	lines_[7].end = positions[7];

	lines_[8].start = positions[5];
	lines_[8].end = positions[1];

	lines_[9].start = positions[6];
	lines_[9].end = positions[4];

	lines_[9].start = positions[6];
	lines_[9].end = positions[4];

	lines_[10].start = positions[6];
	lines_[10].end = positions[7];

	lines_[11].start = positions[6];
	lines_[11].end = positions[2];

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].start = center_;
		orientationLines_[i].end = (orientations_[i] * size_[i]) * worldMatrix_;
	}

	for (auto& line : lines_) {
		line.start *= worldMatrix_;
		line.end *= worldMatrix_;
	}
}

void Obb::Draw(const Mat4x4& viewProjection) {
	for (auto& line : lines_) {
		line.color = color_;
		line.Draw(viewProjection);
	}

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].color = Vector4ToUint(Vector4{ orientations_[i], 1.0f });
		orientationLines_[i].Draw(viewProjection);
	}
}

void Obb::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("center", &center_.x, 0.01f);
	ImGui::DragFloat3("size", &scale_.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	static Vector4 colorEdit;
	colorEdit = UintToVector4(color_);
	ImGui::ColorEdit4("color", colorEdit.m.data());
	color_ = Vector4ToUint(colorEdit);
	ImGui::End();
#endif // _DEBUG
}