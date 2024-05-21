#include "ItemGauge.h"
#include <Drawers/DrawerManager.h>
#include<imgui.h>

void ItemGauge::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	gaugeState_ = std::make_unique<Tex2DState>();
	gaugeState_->color = 0xffffffff;
	gaugeState_->transform.scale = { 1040.0f,96.0f };
	gaugeState_->transform.translate = { 0.0f, 300.0f };
	gaugeState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeFrame.png");

	moveGaugeLeftState_ = std::make_unique<Tex2DState>();
	moveGaugeLeftState_->color = 0xffffffff;
	moveGaugeLeftState_->transform.scale = { 48.0f,64.0f };
	moveGaugeLeftState_->transform.translate = { -300.0f, 300.0f };
	moveGaugeLeftState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSide.png");

	moveGaugeCenterState_ = std::make_unique<Tex2DState>();
	moveGaugeCenterState_->color = 0xffffffff;
	moveGaugeCenterState_->transform.scale = { 0.0f,96.0f };
	moveGaugeCenterState_->transform.translate = { 0.0f, 300.0f };
	moveGaugeCenterState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeCenter.png");

}

void ItemGauge::Update(const int32_t& nowCount, const int32_t& maxCount){
	num_ = static_cast<float>(nowCount) / static_cast<float>(maxCount);

	MoveGauge();

	gaugeState_->transform.CalcMatrix();
	moveGaugeLeftState_->transform.CalcMatrix();
	moveGaugeCenterState_->transform.CalcMatrix();
}

void ItemGauge::Draw(const Camera& camera) const{

	if (num_ > 0.02f) {
		tex2D_->Draw(moveGaugeCenterState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
			, moveGaugeCenterState_->textureID, moveGaugeCenterState_->color, BlendType::kNormal);
	}
	tex2D_->Draw(moveGaugeLeftState_->transform.matWorld_,Mat4x4::kIdentity, camera.GetViewOthographics()
		, moveGaugeLeftState_->textureID, moveGaugeLeftState_->color, BlendType::kNormal);

	
	tex2D_->Draw(gaugeState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, gaugeState_->textureID, gaugeState_->color, BlendType::kNormal);
}

void ItemGauge::Debug(){
	ImGui::Begin("ゲージ関連");
	ImGui::DragFloat("ゲージ増加割合", &num_, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("縁の座標", gaugeState_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("縁の大きさ", gaugeState_->transform.scale.data(), 1.0f);
	ImGui::DragFloat3("ゲージ本体左の座標", moveGaugeLeftState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat2("ゲージ本体左の大きさ", moveGaugeLeftState_->transform.scale.data(), 0.1f);
	ImGui::DragFloat3("ゲージ本体中央の座標", moveGaugeCenterState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat2("ゲージ本体中央の大きさ", moveGaugeCenterState_->transform.scale.data(), 0.1f);
	ImGui::End();
}

void ItemGauge::MoveGauge(){
	float& leftPos = moveGaugeLeftState_->transform.translate.x;

	leftPos = kGaugePosX_.x - (kGaugePosLength_ * (num_));

	moveGaugeCenterState_->transform.translate.x = kGaugeCenterPosX_.x - (kGaugePosCenterLength_ * (num_));
	moveGaugeCenterState_->transform.scale.x = (kGaugeScale_ * (num_));
	if (num_ >= 0.02f && num_ < 0.04f) {
		moveGaugeCenterState_->transform.scale.y = 64.0f;
	}
	else {
		moveGaugeCenterState_->transform.scale.y = 96.0f;
	}
}
