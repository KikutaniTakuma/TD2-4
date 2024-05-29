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
	float scale = Lamb::ClientSize().y / 5.0f * 2.0f;
	for (float index = -2.0f; auto & i : starTransform_) {
		i = std::make_unique<Transform>();
		i->scale *= scale;
		index++;
	}
	starPositions_[0].first.y = offset.y * -2.0f;
	starPositions_[1].first.y = offset.y * -1.0f;
	starPositions_[2].first.y = offset.y * 0.0f;
	starPositions_[3].first.y = offset.y * 1.0f;
	starPositions_[4].first.y = offset.y * 2.0f;

	starPositions_[0].second.y = offset.y * -2.0f;
	starPositions_[1].second.y = offset.y * -1.0f;
	starPositions_[2].second.y = offset.y * 0.0f;
	starPositions_[3].second.y = offset.y * 1.0f;
	starPositions_[4].second.y = offset.y * 2.0f;

	starPositions_[0].first.x = offset.x * -2.0f - Lamb::ClientSize().x;
	starPositions_[1].first.x = offset.x * -1.0f - Lamb::ClientSize().x;
	starPositions_[2].first.x = offset.x *  0.0f - Lamb::ClientSize().x;
	starPositions_[3].first.x = offset.x *  1.0f - Lamb::ClientSize().x;
	starPositions_[4].first.x = offset.x *  2.0f - Lamb::ClientSize().x;

	starPositions_[0].second.x = offset.x *  1.0f + Lamb::ClientSize().x;
	starPositions_[1].second.x = offset.x *  1.0f + Lamb::ClientSize().x;
	starPositions_[2].second.x = offset.x *  1.0f + Lamb::ClientSize().x;
	starPositions_[3].second.x = offset.x *  1.0f + Lamb::ClientSize().x;
	starPositions_[4].second.x = offset.x *  1.0f + Lamb::ClientSize().x;

	for (size_t index = 0; auto & i : fadePositions_) {
		i.max = Lamb::ClientSize().x * 0.7f;
		i.min = 0.0f;

		index++;
	}


	starTexID_ = DrawerManager::GetInstance()->LoadTexture("./Resources/FadeStar.png");
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
		color_ = ColorLerp(fadeOutColor_, fadeInColor_, ease_.GetT());
	}
	else if (isOutStart_) {
		color_ = ColorLerp(fadeInColor_, fadeOutColor_, ease_.GetT());
	}

	ease_.Update();
	transform_->CalcMatrix();

	for (size_t i = 0; i < kStarNum; i++) {
		if (isInStart_) {
			starTransform_[i]->translate = Vector3::Lerp(-starPositions_[i].first, -starPositions_[i].second, ease_.GetT());
		}
		else if (isOutStart_) {
			starTransform_[i]->translate = Vector3::Lerp(starPositions_[i].first, starPositions_[i].second, ease_.GetT());
		}
		starTransform_[i]->CalcMatrix();
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

		for (size_t index = 0; auto & i : starTransform_) {
			float startPos = Lamb::ClientSize().x * -0.5f;
			float xSize = i->translate.x - startPos - (i->scale.x * 0.5f) + 10.0f;
			xSize = std::max(0.0f, xSize);
			tex_->Draw(
				Mat4x4::MakeTranslate(Vector3::kXIdentity * 0.5f) * 
				Mat4x4::MakeAffin(
					Vector3(xSize, i->scale.y, i->scale.z),
					Vector3::kZero, 
					Vector3(Lamb::ClientSize().x * -0.5f, i->translate.y, i->translate.z)
				),
				Mat4x4::kIdentity,
				viewProjection, 0, 0xff, BlendType::kUnenableDepthNone
			);
			index++;
		}

		if (isOutStart_) {
			for (auto i = starTransform_.rbegin(); i != starTransform_.rend(); i++) {
				tex_->Draw(
					(*i)->matWorld_,
					//Mat4x4::MakeScalar(Vector3::kXIdentity * 0.333f + Vector3::kYIdentity * (isInStart_ ? -1.0f : 1.0f)),
					Mat4x4::kIdentity,
					viewProjection, starTexID_, std::numeric_limits<uint32_t>::max(), BlendType::kUnenableDepthNormal
				);
			}
		}
		else {
			for (auto i = starTransform_.begin(); i != starTransform_.end(); i++) {
				tex_->Draw(
					(*i)->matWorld_,
					//Mat4x4::MakeScalar(Vector3::kXIdentity * 0.333f + Vector3::kYIdentity * (isInStart_ ? -1.0f : 1.0f)),
					Mat4x4::kIdentity,
					viewProjection, starTexID_, std::numeric_limits<uint32_t>::max(), BlendType::kUnenableDepthNormal
				);
			}
		}
	}
}