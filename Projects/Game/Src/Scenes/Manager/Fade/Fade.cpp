#include "Fade.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "imgui.h"

Fade::Fade():
	isInStart_(false),
	isOutStart_(false),
	//tex_(),
	fadeTime_(0.75f)
{
	//tex_.scale = Lamb::ClientSize();
}

void Fade::OutStart() {
	if (!isInStart_ && !isOutStart_) {
		isOutStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
void Fade::InStart() {
	if (!isInStart_ && !isOutStart_) {
		isInStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
bool Fade::OutEnd() const {
	return isOutStart_.OnExit();
}
bool Fade::OutStay() const {
	return isOutStart_.OnStay();
}

bool Fade::InEnd() const {
	return isInStart_.OnExit();
}
bool Fade::InStay() const {
	return isInStart_.OnStay();
}

bool Fade::IsActive() const
{
	return isInStart_ || isOutStart_;
}

void Fade::Update() {
	if (isInStart_) {
		//tex_.color = ColorLerp(0xff, 0x00, ease_.GetT());
	}
	else if (isOutStart_) {
		//tex_.color = ColorLerp(0x00, 0xff, ease_.GetT());
	}

	//tex_.Update();

	ease_.Update();

	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
}
void Fade::Draw([[maybe_unused]]const Mat4x4& viewProjection) {
	if (isInStart_ || isOutStart_) {
		//tex_.Draw(viewProjection, Pipeline::Blend::Normal, false);
	}
}