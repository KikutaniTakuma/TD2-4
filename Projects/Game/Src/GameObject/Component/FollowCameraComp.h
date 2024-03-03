#pragma once
#include "../GameObject.h"
#include "Camera/Camera.h"
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "../SoLib/Containers/VItem.h"

class FollowCameraComp : public IComponent {
public:
	using IComponent::IComponent;
	~FollowCameraComp() = default;

	void Init() override;

	void Update() override;

	void ImGuiWidget() override;

	void AddRotate(const Vector3 &euler);

	void SetTarget(Transform *const target);

	const auto &GetCamera() const { return camera_; }

	SoLib::VItem<"LerpValue", float> vLerpValue = 0.85f;

private:


	Transform *pTarget_ = nullptr;

	SoLib::VItem<"Offset", Vector3> offset_{ { 0.f,6.f,-30.f } };
	SoLib::VItem<"AddOffset", Vector3> addOffset_{ { 0.f,6.f,0.f } };

	Vector3 rotate_;
	Vector3 prePos_;

	Camera *camera_;
};