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

	pos *= Mat4x4::MakeAffin(Vector3::kIdentity, rotate_, center_).Inverse();

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

Mat4x4 Obb::GetRotate() const{
	Mat4x4 rotateMatrix;
	rotateMatrix = Mat4x4::kIdentity;
	rotateMatrix[0][0] = orientations_[0].x;
	rotateMatrix[0][1] = orientations_[0].y;
	rotateMatrix[0][2] = orientations_[0].z;
	rotateMatrix[1][0] = orientations_[1].x;
	rotateMatrix[1][1] = orientations_[1].y;
	rotateMatrix[1][2] = orientations_[1].z;
	rotateMatrix[2][0] = orientations_[2].x;
	rotateMatrix[2][1] = orientations_[2].y;
	rotateMatrix[2][2] = orientations_[2].z;
	return rotateMatrix;
}

bool Obb::OBBinPoint(const Vector3& point){
	// OBBのローカル座標系で点を表す
	Vector3 localPoint = (point - center_) * 2.0f;
	// 各軸に沿った境界条件を確認する
	for (int i = 0; i < 3; ++i) {
		float projection = localPoint.Dot(orientations_[i]);
		if ((projection > scale_.x || projection < -scale_.x) && i == 0) {
			isCollision_ = false;
			return false; // 点がOBBの範囲外にある場合
		}
		if ((projection > scale_.y || projection < -scale_.y) && i == 1) {
			isCollision_ = false;
			return false; // 点がOBBの範囲外にある場合
		}
		if ((projection > scale_.z || projection < -scale_.z) && i == 2) {
			isCollision_ = false;
			return false; // 点がOBBの範囲外にある場合
		}
	}
	isCollision_ = true;
	return true; // 点がOBBの範囲内にある場合
}

bool Obb::IsCollisionOBBOBB(const Obb& obb) {

	Vector3 separatingAxis[15]{};
	separatingAxis[0] = orientations_[0];
	separatingAxis[1] = orientations_[1];
	separatingAxis[2] = orientations_[2];
	separatingAxis[3] = obb.orientations_[0];
	separatingAxis[4] = obb.orientations_[1];
	separatingAxis[5] = obb.orientations_[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++) {
		for (int index2 = 0; index2 < 3; index2++) {
			separatingAxis[axisNum] = orientations_[index1].Cross(obb.orientations_[index2].Normalize());
			axisNum++;
		}
	}

	Vector3 obb1Vertex[8]{};
	// bottom
	obb1Vertex[0] = Vector3{ -scale_.x, -size_.y, -scale_.z };
	obb1Vertex[1] = Vector3{ +scale_.x, -size_.y, -scale_.z };
	obb1Vertex[2] = Vector3{ -scale_.x, -size_.y, +scale_.z };
	obb1Vertex[3] = Vector3{ +scale_.x, -size_.y, +scale_.z };
	// top
	obb1Vertex[4] = Vector3{ -scale_.x, +size_.y, -scale_.z };
	obb1Vertex[5] = Vector3{ +scale_.x, +size_.y, -scale_.z };
	obb1Vertex[6] = Vector3{ -scale_.x, +size_.y, +scale_.z };
	obb1Vertex[7] = Vector3{ +scale_.x, +size_.y, +scale_.z };

	Mat4x4 rotateMatrix1 = GetRotate();

	Vector3 obb2Vertex[8]{};
	// bottom
	obb2Vertex[0] = Vector3{ -obb.scale_.x, -obb.GetSize().y, -obb.scale_.z};
	obb2Vertex[1] = Vector3{ +obb.scale_.x, -obb.GetSize().y, -obb.scale_.z };
	obb2Vertex[2] = Vector3{ -obb.scale_.x, -obb.GetSize().y, +obb.scale_.z };
	obb2Vertex[3] = Vector3{ +obb.scale_.x, -obb.GetSize().y, +obb.scale_.z };
	// top
	obb2Vertex[4] = Vector3{ -obb.scale_.x, +obb.GetSize().y, -obb.scale_.z };
	obb2Vertex[5] = Vector3{ +obb.scale_.x, +obb.GetSize().y, -obb.scale_.z };
	obb2Vertex[6] = Vector3{ -obb.scale_.x, +obb.GetSize().y, +obb.scale_.z };
	obb2Vertex[7] = Vector3{ +obb.scale_.x, +obb.GetSize().y, +obb.scale_.z };

	Mat4x4 rotateMatrix2 = obb.GetRotate();

	for (int index = 0; index < 8; index++) {
		obb1Vertex[index] = obb1Vertex[index] * rotateMatrix1;
		obb1Vertex[index] = (obb1Vertex[index] + center_);
		obb2Vertex[index] = (obb2Vertex[index] * rotateMatrix2);
		obb2Vertex[index] = (obb2Vertex[index] + obb.center_);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8]{};
		float projectionPoint2[8]{};
		float min1, max1;
		float min2, max2;
		min1 = (obb1Vertex[0].Dot(separatingAxis[axisNum].Normalize()));
		min2 = (obb2Vertex[0].Dot(separatingAxis[axisNum].Normalize()));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
				obb1Vertex[index].Dot(separatingAxis[axisNum].Normalize());
			projectionPoint2[index] =
				obb2Vertex[index].Dot(separatingAxis[axisNum].Normalize());
			if (index == 0) {
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else {
				min1 = std::min(min1, projectionPoint1[index]);
				min2 = std::min(min2, projectionPoint2[index]);
				max1 = std::max(max1, projectionPoint1[index]);
				max2 = std::max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = std::max(max1, max2) - std::min(min1, min2);
		if (sumSpan < longSpan) {
			isCollision_ = false;
			return false;
		}
	}
	isCollision_ = true;
	return true;
}


void Obb::Update() {
	worldMatrix_= Mat4x4::MakeAffin(scale_, rotate_, center_);

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