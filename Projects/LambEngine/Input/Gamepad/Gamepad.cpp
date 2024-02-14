#include "GamePad.h"
#include "imgui.h"
#include <limits>
#include <algorithm>
#include "Math/Vector2.h"

Gamepad::Gamepad() :
	preButton_(0),
	state_({0}),
	vibration_({0})
{}

Gamepad* const Gamepad::GetInstance() {
	static Gamepad pInstance;
	return &pInstance;
}

void Gamepad::Input() {
	preButton_ = state_.Gamepad.wButtons;
    XInputGetState(0, &state_);
}

void Gamepad::InputReset() {
	preButton_ = 0;
	state_ = {  };
	vibration_ = {  };
}

bool Gamepad::GetButton(Button type) {
    return (state_.Gamepad.wButtons >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::GetPreButton(Button type) {
	return (preButton_ >> static_cast<short>(type)) % 2 == 1;
}

bool Gamepad::Pushed(Button type) {
	return GetButton(type) && !GetPreButton(type);
}

bool Gamepad::LongPushed(Button type) {
	return GetButton(type) && GetPreButton(type);
}

bool Gamepad::Released(Button type) {
	return !GetButton(type) && GetPreButton(type);
}

bool Gamepad::PushAnyKey() {
	Gamepad* instance = Gamepad::GetInstance();
	float leftStickX = GetStick(Stick::LEFT_X);
	float leftStickY = GetStick(Stick::LEFT_Y);
	float rightStickX = GetStick(Stick::RIGHT_X);
	float rightStickY = GetStick(Stick::RIGHT_Y);

	if (instance->state_.Gamepad.bLeftTrigger
		|| instance->state_.Gamepad.bRightTrigger
		|| leftStickX < -0.3f || 0.3f <leftStickX
		|| leftStickY < -0.3f || 0.3f <leftStickY
		|| rightStickX < -0.3f || 0.3f < rightStickX
		|| rightStickY < -0.3f || 0.3f < rightStickY
		) {
		return true;
	}

	return instance->state_.Gamepad.wButtons != instance->preButton_;
}

float Gamepad::GetTriger(Triger type, float deadZone) {
	static constexpr float kNormal = 1.0f / static_cast<float>(UCHAR_MAX);
	float moveTriger = 0.0f;
	switch (type)
	{
	case Gamepad::Triger::LEFT:
		moveTriger = static_cast<float>(state_.Gamepad.bLeftTrigger) * kNormal;
		break;

	case Gamepad::Triger::RIGHT:
		moveTriger = static_cast<float>(state_.Gamepad.bRightTrigger) * kNormal;
		break;

	default:
		return 0.0f;
	}

	return moveTriger <= deadZone ? 0.0f : moveTriger;
}

float Gamepad::GetStick(Stick type, float deadZone) {
	static constexpr float kNormal = 1.0f / static_cast<float>(SHRT_MAX);
	float moveStick = 0.0f;
	deadZone = std::clamp(deadZone, 0.0f, 1.0f);

	switch (type)
	{
	case Gamepad::Stick::LEFT_X:
		moveStick = static_cast<float>(state_.Gamepad.sThumbLX) * kNormal;
		break;
	case Gamepad::Stick::LEFT_Y:
		moveStick = static_cast<float>(state_.Gamepad.sThumbLY) * kNormal;
		break;
	case Gamepad::Stick::RIGHT_X:
		moveStick = static_cast<float>(state_.Gamepad.sThumbRX) * kNormal;
		break;
	case Gamepad::Stick::RIGHT_Y:
		moveStick = static_cast<float>(state_.Gamepad.sThumbRY) * kNormal;
		break;
	default:
		return 0.0f;
	}

	// もしデッドゾーン内だった場合は0.0fを返す
	if (-deadZone <= moveStick && moveStick <= deadZone) {
		return 0.0f;
	}

	return moveStick;
}

void Gamepad::Vibration(float leftVibIntensity, float rightVibIntensity) {
	leftVibIntensity = std::clamp(leftVibIntensity, 0.0f, 1.0f);
	rightVibIntensity = std::clamp(rightVibIntensity, 0.0f, 1.0f);

	vibration_.wLeftMotorSpeed = static_cast<WORD>(static_cast<float>(USHRT_MAX) * leftVibIntensity);
	vibration_.wRightMotorSpeed = static_cast<WORD>(static_cast<float>(USHRT_MAX) * rightVibIntensity);
	XInputSetState(0, &vibration_);
}

void Gamepad::Debug() {
#ifdef _DEBUG
	ImGui::SetNextWindowSizeConstraints({}, { 210.0f, 400.0f });
	ImGui::Begin("Gamepad Debug");
	if (ImGui::TreeNode("stick")) {
		ImGui::Text("LeftX          = %.2f%%\n", static_cast<float>(GetStick(Stick::LEFT_X)) * 100.0f);
		ImGui::Text("LeftY          = %.2f%%\n", static_cast<float>(GetStick(Stick::LEFT_Y)) * 100.0f);
		ImGui::Text("RightX         = %.2f%%\n", static_cast<float>(GetStick(Stick::RIGHT_X)) * 100.0f);
		ImGui::Text("RightY         = %.2f%%\n", static_cast<float>(GetStick(Stick::RIGHT_Y)) * 100.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("triger")) {
		ImGui::Text("LEFT_TRIGER    = %.2f%%\n", GetTriger(Triger::LEFT) * 100.0f);
		ImGui::Text("RIGHT_TRIGER   = %.2f%%\n", GetTriger(Triger::RIGHT) * 100.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ABXY")) {
		ImGui::Text("A              = %d\n", GetButton(Button::A));
		ImGui::Text("B              = %d\n", GetButton(Button::B));
		ImGui::Text("X              = %d\n", GetButton(Button::X));
		ImGui::Text("Y              = %d\n", GetButton(Button::Y));
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("cross button")) {
		ImGui::Text("UP             = %d\n", GetButton(Button::UP));
		ImGui::Text("DOWN           = %d\n", GetButton(Button::DOWN));
		ImGui::Text("LEFT           = %d\n", GetButton(Button::LEFT));
		ImGui::Text("RIGHT          = %d\n", GetButton(Button::RIGHT));
		ImGui::TreePop();
	}	
	if (ImGui::TreeNode("other")) {
		ImGui::Text("START          = %d\n", GetButton(Button::START));
		ImGui::Text("BACK           = %d\n", GetButton(Button::BACK));
		ImGui::Text("LEFT_THUMB     = %d\n", GetButton(Button::LEFT_THUMB));
		ImGui::Text("RIGHT_THUMB    = %d\n", GetButton(Button::RIGHT_THUMB));
		ImGui::Text("LEFT_SHOULDER  = %d\n", GetButton(Button::LEFT_SHOULDER));
		ImGui::Text("RIGHT_SHOULDER = %d\n", GetButton(Button::RIGHT_SHOULDER));
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("vibration")) {
		static Vector2 vibration;
		ImGui::DragFloat2("Vib", &vibration.x, 0.01f, 0.0f, 1.0f);
		Gamepad::Vibration(vibration.x, vibration.y);
		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}