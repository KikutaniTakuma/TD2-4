#include "FollowCameraComp.h"

void FollowCameraComp::Init() {

}

void FollowCameraComp::Update() {

	Vector3 linePoint = prePos_;
	//SoLib::Lerp<Vector3>(line_.ClosestPoint(pTarget_->GetGrobalPos()), prePos_, vLerpValue);


	if (rotate_.x > 15._deg) {
		rotate_.x = SoLib::Lerp(rotate_.x, 15._deg, 0.2f);
	}
	else if (rotate_.x < -40._deg) {
		rotate_.x = SoLib::Lerp(rotate_.x, -40._deg, 0.2f);
	}

	SoLib::ImGuiText("FollowCameraAngle", SoLib::to_string(rotate_));

	const Vector3 cameraOffset = *offset_ * Mat4x4::MakeRotate(rotate_);

	camera_->pos = cameraOffset + SoLib::Lerp(pTarget_->GetGrobalPos(), linePoint, vLerpValue);

	Vector3 facing = linePoint - camera_->GetPos();

	camera_->rotate = rotate_;

	camera_->Update();

	prePos_ = linePoint;
}

void FollowCameraComp::ImGuiWidget() {
	SoLib::ImGuiWidget(&vLerpValue);
	SoLib::ImGuiWidget(&offset_);
	SoLib::ImGuiWidget(&addOffset_);
}

void FollowCameraComp::AddRotate(const Vector3 &euler) {
	if (euler.LengthSQ() != 0.f) {
		rotate_ += euler;

		camera_->rotate += euler;
		camera_->Update();
	}
}

void FollowCameraComp::SetTarget(Transform *const target) {
	pTarget_ = target;
}