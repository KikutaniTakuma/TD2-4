#include "Camera.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Input/Input.h"
#include "imgui.h"
#include <algorithm>
#include <numbers>
#include <cmath>

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

Camera::Camera(const Camera& right) noexcept
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept
{
	*this = std::move(right);
}

void Camera::Update() {
	view_.Affin(scale, rotate, pos);
	worldPos_ = pos;
	view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;


	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_.PerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_.Orthographic(
		-clientSize.x * 0.5f * drawScale,
		clientSize.y * 0.5f * drawScale,
		clientSize.x * 0.5f * drawScale,
		-clientSize.y * 0.5f * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::Update(const Vector3& gazePoint) {
	pos = gazePoint + (offset * Mat4x4::MakeRotate(rotate));

	Update();
}

void Camera::Update(const Mat4x4& worldMat) {
	view_.Affin(scale, rotate, pos);
	view_ = worldMat * view_;
	worldPos_ = { view_[0][3],view_[1][3], view_[2][3] };
	view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;

	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_.PerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_.Orthographic(
		-clientSize.x * 0.5f * drawScale,
		clientSize.y * 0.5f * drawScale,
		clientSize.x * 0.5f * drawScale,
		-clientSize.y * 0.5f * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("offset", &offset.x);
	ImGui::End();
#endif // _DEBUG
}