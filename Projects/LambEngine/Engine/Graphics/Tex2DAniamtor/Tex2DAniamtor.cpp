#include "Tex2DAniamtor.h"
#include "Utils/EngineInfo/EngineInfo.h"

void Tex2DAniamtor::Update() {

	if ((duration_ * static_cast<float>(currentAnimationNumber_) < animationTime_)) {
		currentAnimationNumber_++;
		currentPos_.x += std::abs(scale_.x);
	}

	if (isActive_) {
		animationTime_ += Lamb::DeltaTime();
	}

	if (animationNumber_ <= currentAnimationNumber_) {
		if (isLoop_) {
			Reset();
		}
		else {
			isActive_ = false;
		}
	}

	animationMatrix_ = Mat4x4::MakeAffin(scale_, Vector3::kZero, currentPos_);
}

void Tex2DAniamtor::Start() {
	Reset();
	Restart();
}

void Tex2DAniamtor::Stop() {
	Pause();
	Reset();
}

void Tex2DAniamtor::Reset() {
	currentAnimationNumber_ = 0;
	currentPos_ = startPos_;
	animationTime_ = 0.0f;
}

void Tex2DAniamtor::Restart() {
	isActive_ = true;
}

void Tex2DAniamtor::Pause() {
	isActive_ = false;
}

void Tex2DAniamtor::SetAnimationNumber(uint32_t animationNumber) {
	animationNumber_ = std::max(1u, animationNumber);
	scale_.x = 1.0f / static_cast<float>(animationNumber_);
}

void Tex2DAniamtor::FlipUpsideDown(bool isSingned) {
	if ((isSingned and 0.0f < scale_.y) or (not isSingned and scale_.y < 0.0f)) {
		scale_.y *= -1.0f;
	}
}

void Tex2DAniamtor::FlipHorizontal(bool isSingned) {
	if ((isSingned and 0.0f < scale_.x) or (not isSingned and scale_.x < 0.0f)) {
		scale_.x *= -1.0f;
	}
}
