#include "SimpleFade.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "imgui.h"

#include <climits>

SimpleFade::SimpleFade() :
	isInStart_(false),
	isOutStart_(false),
	tex_(),
	transform_(),
	color_(std::numeric_limits<uint32_t>::max()),
	fadeTime_(0.75f)
{
	tex_ = DrawerManager::GetInstance()->GetTexture2D();
	transform_ = std::make_unique<Transform>();
	transform_->scale = Lamb::ClientSize();
	transform_->translate.z = -10.0f + static_cast<float>(10e-5);
	fadeInColor_ = 0x00000000;
	fadeOutColor_ = 0x000000FF;
}

void SimpleFade::OutStart() {
	if (!isInStart_ && !isOutStart_) {
		isOutStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
void SimpleFade::InStart() {
	if (!isInStart_ && !isOutStart_) {
		isInStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
bool SimpleFade::OutEnd() const {
	return isOutStart_.OnExit();
}
bool SimpleFade::OutStay() const {
	return isOutStart_.OnStay();
}

bool SimpleFade::InEnd() const {
	return isInStart_.OnExit();
}
bool SimpleFade::InStay() const {
	return isInStart_.OnStay();
}

bool SimpleFade::IsActive() const
{
	return isInStart_ || isOutStart_;
}

void SimpleFade::Update() {
	if (isInStart_) {
		color_ = ColorLerp(fadeOutColor_, fadeInColor_, ease_.GetT());
	}
	else if (isOutStart_) {
		color_ = ColorLerp(fadeInColor_, fadeOutColor_, ease_.GetT());
	}

	ease_.Update();
	transform_->CalcMatrix();

	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
}
void SimpleFade::Draw([[maybe_unused]] const Mat4x4& viewProjection) {
	if (isInStart_ || isOutStart_) {
		tex_->Draw(transform_->matWorld_, Mat4x4::kIdentity, viewProjection, 0u, color_, BlendType::kNormal);
	}
}