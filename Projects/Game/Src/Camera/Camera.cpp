#include "Camera.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Input/Input.h"
#include "imgui.h"
#include <algorithm>
#include <numbers>
#include <cmath>
#include<LambEngine/Utils/Random/Random.h>

#include "../SoLib/SoLib/SoLib_ImGui.h"

Camera::Camera() noexcept :
	pos(),
	scale(Vector3::kIdentity),
	rotate(),
	drawScale(1.0f),
	farClip(1000.0f),
	fov(0.45f),
	view_(),
	projection_(),
	othograohics_(),
	offset{ Vector3::kZIdentity * -10.0f }
{}

Camera::Camera(const Camera &right) noexcept
{
	*this = right;
}

Camera::Camera(Camera &&right) noexcept
{
	*this = std::move(right);
}

void Camera::Update() {
	view_ = Mat4x4::MakeAffin(scale, rotate, pos);
	worldPos_ = pos;
	view_ = view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;


	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_ = Mat4x4::MakePerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_ = Mat4x4::MakeOrthographic(
		-clientSize.x * 0.5f * drawScale,
		clientSize.y * 0.5f * drawScale,
		clientSize.x * 0.5f * drawScale,
		-clientSize.y * 0.5f * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::Update(const Vector3 &gazePoint) {
	pos = gazePoint + (offset * Mat4x4::MakeRotate(rotate));

	Update();
}

void Camera::Update(const Mat4x4& worldMat) {
	view_ = Mat4x4::MakeAffin(scale, rotate, pos);
	view_ = worldMat * view_;
	worldPos_ = { view_[0][3],view_[1][3], view_[2][3] };
	view_ = view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;

	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_= Mat4x4::MakePerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_= Mat4x4::MakeOrthographic(
		-clientSize.x * 0.5f * drawScale,
		clientSize.y * 0.5f * drawScale,
		clientSize.x * 0.5f * drawScale,
		-clientSize.y * 0.5f * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::BeginShake(const Vector2& power){
	if (!isShake_){
		isShake_ = true;
		shakePower_ = power;
		shakePowerBase_ = power;
		shakeBasePos_ = Vector2{ pos.x,pos.y };
	}

	if (isShake_) {
		isShake_ = true;
		shakePower_ = power;
		shakePowerBase_ = power;
	}
	
}

void Camera::VerticalShake(const float time){
	if (isShake_){
		if (shakePower_.y < 0.01f && shakePower_.y > -0.01f) {
			isShake_ = false;
			return;
		}
		pos.y = shakeBasePos_.y + shakePower_.y;
		shakePower_.y *= -1.0f;
		if (shakePower_.y>0){
			shakePower_.y -= (shakePowerBase_.y / (time * 60.0f));
		}
		else if (shakePower_.y < 0) {
			shakePower_.y += (shakePowerBase_.y / (time * 60.0f));
		}

	}

}

void Camera::HorizontalShake(const float time){
	if (isShake_) {
		if (shakePower_.x < 0.01f && shakePower_.x > -0.01f) {
			isShake_ = false;
			return;
		}
		pos.x = shakeBasePos_.x + shakePower_.x;
		shakePower_.x *= -1.0f;
		if (shakePower_.x > 0) {
			shakePower_.x -= (shakePowerBase_.x / (time * 60.0f));
		}
		else if (shakePower_.x < 0) {
			shakePower_.x += (shakePowerBase_.x / (time * 60.0f));
		}

	}
}

void Camera::Shake(const float time){
	if (isShake_) {
		if (shakePower_.Length() < 0.01f) {
			isShake_ = false;
			return;
		}
		Vector2 shakeVec;

		shakeVec.x = Lamb::Random(-shakePower_.x, shakePower_.x);
		shakeVec.y = Lamb::Random(-shakePower_.y, shakePower_.y);

		pos = shakeBasePos_ + shakeVec;
		
		if (shakePower_.y > 0) {
			shakePower_.y -= (shakePowerBase_.y / (time * 60.0f));
		}
		else if (shakePower_.y < 0) {
			shakePower_.y += (shakePowerBase_.y / (time * 60.0f));
		}

		if (shakePower_.x > 0) {
			shakePower_.x -= (shakePowerBase_.x / (time * 60.0f));
		}
		else if (shakePower_.x < 0) {
			shakePower_.x += (shakePowerBase_.x / (time * 60.0f));
		}

	}
}

void Camera::Debug([[maybe_unused]] const std::string &guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	SoLib::ImGuiDragEuler("rotate", &rotate.x);
	ImGui::DragFloat3("offset", &offset.x);
	ImGui::DragFloat("drawScale", &drawScale);
	ImGui::End();
#endif // _DEBUG
}