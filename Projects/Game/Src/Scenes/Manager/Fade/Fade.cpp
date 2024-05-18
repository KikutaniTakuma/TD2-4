#include "Fade.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "imgui.h"

#include <climits>

Fade::Fade() :
	isInStart_(false),
	isOutStart_(false),
	tex_(),
	transform_(),
	color_(std::numeric_limits<uint32_t>::max()),
	fadeTime_(1.5f)
{
	tex_ = DrawerManager::GetInstance()->GetTexture2D();
	transform_ = std::make_unique<Transform>();
	transform_->scale = Lamb::ClientSize();
	transform_->translate.z = 0.0f;

	Vector2 offset = Lamb::ClientSize() / 5.0f;
	float scale = Lamb::ClientSize().x / 5.0f * 3.0f;
	for (float index = -2.0f; auto & i : slimeTransform_) {
		i = std::make_unique<Transform>();
		i->scale *= scale;
		index++;
	}
	slimePositions_[0].first.x = offset.x * -2.0f;
	slimePositions_[1].first.x = offset.x * 2.0f;
	slimePositions_[2].first.x = offset.x * -1.0f;
	slimePositions_[3].first.x = offset.x * 1.0f;
	slimePositions_[4].first.x = offset.x * 0.0f;

	slimePositions_[0].second.x = offset.x * -2.0f;
	slimePositions_[1].second.x = offset.x * 2.0f;
	slimePositions_[2].second.x = offset.x * -1.0f;
	slimePositions_[3].second.x = offset.x * 1.0f;
	slimePositions_[4].second.x = offset.x * 0.0f;

	slimePositions_[0].first.y = offset.y * -2.0f - Lamb::ClientSize().y;
	slimePositions_[1].first.y = offset.y * -2.0f - Lamb::ClientSize().y;
	slimePositions_[2].first.y = offset.y * -1.0f - Lamb::ClientSize().y;
	slimePositions_[3].first.y = offset.y * -1.0f - Lamb::ClientSize().y;
	slimePositions_[4].first.y = offset.y *  0.0f - Lamb::ClientSize().y;

	slimePositions_[0].second.y = offset.y *  2.0f + Lamb::ClientSize().y;
	slimePositions_[1].second.y = offset.y *  2.0f + Lamb::ClientSize().y;
	slimePositions_[2].second.y = offset.y *  1.0f + Lamb::ClientSize().y;
	slimePositions_[3].second.y = offset.y *  1.0f + Lamb::ClientSize().y;
	slimePositions_[4].second.y = offset.y *  0.0f + Lamb::ClientSize().y;


	slimeTexID_ = DrawerManager::GetInstance()->LoadTexture("./Resources/Enemy/slimeWallWalk.png");
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
	transform_->CalcMatrix();

	for (size_t i = 0; i < kSlimeNum; i++) {
		if (isInStart_) {
			slimeTransform_[i]->translate = Vector3::Lerp(slimePositions_[i].second, slimePositions_[i].first, ease_.GetT());
		}
		else if (isOutStart_) {
			slimeTransform_[i]->translate = Vector3::Lerp(slimePositions_[i].first, slimePositions_[i].second, ease_.GetT());
		}
		slimeTransform_[i]->CalcMatrix();
	}

#ifdef _DEBUG
	ImGui::Begin("fade");
	transform_->ImGuiWidget("Fade");
	ImGui::End();
#endif // _DEBUG


	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
}
void Fade::Draw([[maybe_unused]]const Mat4x4& viewProjection) {
	if (isInStart_ or isOutStart_) {
		tex_->Draw(transform_->matWorld_, Mat4x4::kIdentity, viewProjection, 0u, color_, BlendType::kUnenableDepthNormal);


		for (auto& i : slimeTransform_) {
			tex_->Draw(
				i->matWorld_,
				Mat4x4::MakeScalar(Vector3::kXIdentity * 0.333f + Vector3::kYIdentity * (isInStart_ ? -1.0f : 1.0f)),
				viewProjection, slimeTexID_, std::numeric_limits<uint32_t>::max(), BlendType::kUnenableDepthNormal
			);
		}
	}
}