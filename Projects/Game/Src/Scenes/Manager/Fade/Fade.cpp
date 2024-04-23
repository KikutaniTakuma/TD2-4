#include "Fade.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "imgui.h"

#include <climits>

Fade::Fade() :
	isInStart_(false),
	isOutStart_(false),
	tex_(),
	transform(),
	color_(std::numeric_limits<uint32_t>::max()),
	fadeTime_(0.75f)
{
	tex_ = DrawerManager::GetInstance()->GetTexture2D();
	transform = std::make_unique<Transform>();
	transform->scale = Lamb::ClientSize();
	transform->translate.z = 0.1f;
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
		color_ = ColorLerp(0xff, 0x00, ease_.GetT());
	}
	else if (isOutStart_) {
		color_ = ColorLerp(0x00, 0xff, ease_.GetT());
	}

	ease_.Update();

	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
}
void Fade::Draw([[maybe_unused]]const Mat4x4& viewProjection) {
	if (isInStart_ || isOutStart_) {
		tex_->Draw(transform->GetMatrix(), Mat4x4::kIdentity, viewProjection, 0u, color_, BlendType::kNormal);
	}
}