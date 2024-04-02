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
	translateSpeed_(1.f),
	isRotateInverse_(false),
	isTranslateInverse_(false)
{}

void DebugCamera::Update() {
#ifdef _DEBUG
	Debug("DebugCamera");
	Mouse* const mouse = Mouse::GetInstance();
	KeyInput* const key = KeyInput::GetInstance();
	
	bool isOnImGui = ImGui::GetIO().WantCaptureMouse;
	bool isShift = key->LongPush(DIK_LSHIFT) || key->LongPush(DIK_RSHIFT);
	bool isMiddle = mouse->LongPush(Mouse::Button::Middle);
	const Vector2& mouseVelocity = mouse->GetVelocity();
	float rotateSigned = isRotateInverse_ ? 1.0f : -1.0f;
	float translateSigned = isTranslateInverse_ ? 1.0f : -1.0f;

	if (not isOnImGui && isShift && isMiddle) {
		pos.x += mouseVelocity.x * translateSpeed_ * Lamb::DeltaTime() * translateSigned;
		pos.y += mouseVelocity.y * translateSpeed_ * Lamb::DeltaTime() * translateSigned;
	}

	if (not isOnImGui && not isShift && isMiddle) {
		rotate.y += mouseVelocity.x * rotateSpeed_ * Lamb::DeltaTime() * rotateSigned;
		rotate.x += mouseVelocity.y * rotateSpeed_ * Lamb::DeltaTime() * rotateSigned;
	}

	float isSigned = mouse->GetWheelVelocity();
	if (not (isSigned == 0.0f) && not isOnImGui) {
		scale.z += scaleSpeed_ * isSigned;
		scale.z = std::max(0.1f, scale.z);
	}
#endif // _DEBUG
	Camera::Update();
}

void DebugCamera::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	Camera::Debug(guiName);
	ImGui::Begin(guiName.c_str());
	if (ImGui::TreeNode("Debug UI Setting")) {
		ImGui::Checkbox("カメラ回転反転", &isRotateInverse_);
		ImGui::Checkbox("カメラ移動反転", &isTranslateInverse_);
		ImGui::DragFloat("ズーム速度", &scaleSpeed_, 0.01f);
		ImGui::DragFloat("回転速度", &rotateSpeed_,0.01f);
		ImGui::DragFloat("移動速度", &translateSpeed_,0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}
