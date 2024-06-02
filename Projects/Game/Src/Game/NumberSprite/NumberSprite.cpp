#include "NumberSprite.h"
#include "Camera/Camera.h"

uint32_t NumberSprite::numberTextureID_ = 0u;

void NumberSprite::Load() {
	numberTextureID_ = TextureManager::GetInstance()->LoadTexture("./Resources/UI/Timer/timeLimitNumber.png");
}

NumberSprite::NumberSprite() :
	transform_(std::make_unique<Transform>()),
	num_(0),
	digitNum_(1u)
{
	
}

NumberSprite& NumberSprite::operator=(const NumberSprite& right) {
	*this->transform_ = *right.transform_;
	this->num_ = right.num_;

	return *this;
}
NumberSprite& NumberSprite::operator=(NumberSprite&& right) noexcept {
	this->transform_ = std::move(right.transform_);
	this->num_ = std::move(right.num_);

	return *this;
}

void NumberSprite::Update() {
	this->transform_->CalcMatrix();
}

void NumberSprite::Draw(const Camera& camera, BlendType blendTyoe) {
	if (digitNum_ == 0) {
		return;
	}

	Lamb::SafePtr tex2D = DrawerManager::GetInstance()->GetTexture2D();
	Vector3 offset = Vector3(float(digitNum_ - 1) * -0.5f, 0.0f, 0.0f);

	for (uint32_t i = digitNum_; 0 < i; i--) {
		tex2D->Draw(
			Mat4x4::MakeTranslate(offset) * this->transform_->matWorld_,
			Mat4x4::MakeAffin(Vector3(0.1f, 1.0f, 1.0f), Vector3::kZero, Vector3::kXIdentity * static_cast<float>(NumberToDigitNumber(num_, i)) * 0.1f),
			camera.GetViewOthographics(),
			numberTextureID_,
			0xffffffff,
			blendTyoe
		);

		offset.x += 1.0f;
	}
}

void NumberSprite::ImGuiWidget(const std::string& guiName) {
	transform_->ImGuiWidget(guiName);
}

uint32_t NumberToDigitNumber(uint32_t num, uint32_t digit) {
	if (num < 10) {
		return num;
	}
	uint32_t digitOffset = static_cast<uint32_t>(std::pow(10u, digit));
	if (static_cast<uint32_t>(std::log10(num)) + 1 < digit) {
		return 0;
	}
	uint32_t offset = num - ((num / digitOffset) * digitOffset);

	if (digit == 1) {
		return offset;
	}
	
	return (offset - NumberToDigitNumber(offset, digit - 1)) / static_cast<uint32_t>(std::pow(10u, digit - 1));
}