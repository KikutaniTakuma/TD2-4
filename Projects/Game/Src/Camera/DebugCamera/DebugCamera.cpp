#include "DebugCamera.h"
#include "Input/Mouse/Mouse.h"
#include "imgui.h"

#undef max
#undef min

DebugCamera::DebugCamera():
	Camera(),
	scaleSpeed_(0.01f)
{}

void DebugCamera::Update() {
#ifdef _DEBUG
	Mouse* const mouse = Mouse::GetInstance();
	
	bool isOnImGui = ImGui::GetIO().WantCaptureMouse;

	if (not isOnImGui && mouse->LongPush(Mouse::Button::Middle)) {
		const Vector2& mouseVelocity = mouse->GetVelocity();

		pos.x += mouseVelocity.x;
		pos.y -= mouseVelocity.y;
	}

	float isSigned = mouse->GetWheelVelocity();
	if (not (isSigned == 0.0f) && not isOnImGui) {
		scale.z += scaleSpeed_ * isSigned;
		scale.z = std::max(0.1f, scale.z);
	}
#endif // _DEBUG
	Camera::Update();
}
