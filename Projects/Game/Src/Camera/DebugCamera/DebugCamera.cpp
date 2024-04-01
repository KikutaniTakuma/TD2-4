#include "DebugCamera.h"
#include "Input/Input.h"
#include "imgui.h"
#include "Utils/EngineInfo/EngineInfo.h"

#undef max
#undef min

DebugCamera::DebugCamera():
	Camera(),
	scaleSpeed_(0.001f),
	rotateSpeed_(0.01f),
	translateSpeed_(1.f)
{}

void DebugCamera::Update() {
#ifdef _DEBUG
	Mouse* const mouse = Mouse::GetInstance();
	KeyInput* const key = KeyInput::GetInstance();
	
	bool isOnImGui = ImGui::GetIO().WantCaptureMouse;
	bool isShift = key->LongPush(DIK_LSHIFT) || key->LongPush(DIK_RSHIFT);
	bool isMiddle = mouse->LongPush(Mouse::Button::Middle);
	const Vector2& mouseVelocity = mouse->GetVelocity();

	if (not isOnImGui && isShift && isMiddle) {
		pos.x += mouseVelocity.x * translateSpeed_ * Lamb::DeltaTime();
		pos.y -= mouseVelocity.y * translateSpeed_ * Lamb::DeltaTime();
	}

	if (not isOnImGui && not isShift && isMiddle) {
		rotate.y += mouseVelocity.x * rotateSpeed_ * Lamb::DeltaTime();
		rotate.x += mouseVelocity.y * rotateSpeed_ * Lamb::DeltaTime();
	}

	float isSigned = mouse->GetWheelVelocity();
	if (not (isSigned == 0.0f) && not isOnImGui) {
		scale.z += scaleSpeed_ * isSigned;
		scale.z = std::max(0.1f, scale.z);
	}
#endif // _DEBUG
	Camera::Update();
}
