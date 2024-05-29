#include "ItemGauge.h"
#include <Drawers/DrawerManager.h>
#include<imgui.h>

void ItemGauge::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	gaugeState_ = std::make_unique<Tex2DState>();
	gaugeState_->color = 0xffffffff;
	gaugeState_->transform.scale = { 780.0f,72.0f };
	gaugeState_->transform.translate = { -135.0f, 315.0f };
	gaugeState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeFrame.png");

	moveGaugeLeftState_ = std::make_unique<Tex2DState>();
	moveGaugeLeftState_->color = 0x5ea632ff;
	moveGaugeLeftState_->transform.scale = { 36.0f,48.0f };
	moveGaugeLeftState_->transform.translate = { -430.0f, 315.0f };
	moveGaugeLeftState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSideWhite.png");

	moveGaugeRightState_ = std::make_unique<Tex2DState>();
	moveGaugeRightState_->color = 0x5ea632ff;
	moveGaugeRightState_->transform.scale = { 36.0f,48.0f };
	moveGaugeRightState_->transform.translate = { 200.0f, 315.0f };
	moveGaugeRightState_->uvTransform.scale = { -1.0f,1.0f };
	moveGaugeRightState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSideWhite.png");

	moveGaugeCenterState_ = std::make_unique<Tex2DState>();
	moveGaugeCenterState_->color = 0x5ea632ff;
	moveGaugeCenterState_->transform.scale = { 0.0f,72.0f };
	moveGaugeCenterState_->transform.translate = { 0.0f, 315.0f };
	moveGaugeCenterState_->textureID = TextureManager::GetInstance()->GetWhiteTex();

	gaugePosLength_ = kGaugePosX_.x - kGaugePosX_.y;
	gaugePosCenterLength_ = kGaugeCenterPosX_.x - kGaugeCenterPosX_.y;
}

void ItemGauge::Update(const int32_t& nowCount, const int32_t& maxCount){
	isItemReduction_ = false;
	if (afterItemNum != nowCount) {
		if (afterItemNum > nowCount) {
			isItemReduction_ = true;
		}

		beforeItemNum = afterItemNum;
		afterItemNum = nowCount;
	}
	

	num_ = static_cast<float>(nowCount) / static_cast<float>(maxCount);

	if (num_ > 1.0f) {
		num_ = 1.0f;
	}

	MoveGauge();

	gaugeState_->transform.CalcMatrix();
	moveGaugeLeftState_->transform.CalcMatrix();
	moveGaugeRightState_->transform.CalcMatrix();
	moveGaugeRightState_->uvTransform.CalcMatrix();
	moveGaugeCenterState_->transform.CalcMatrix();


}

void ItemGauge::Draw(const Camera& camera) const{

	tex2D_->Draw(gaugeState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, gaugeState_->textureID, gaugeState_->color, BlendType::kNormal);
	

	tex2D_->Draw(moveGaugeRightState_->transform.matWorld_,moveGaugeRightState_->uvTransform.matWorld_, camera.GetViewOthographics()
		, moveGaugeRightState_->textureID, moveGaugeRightState_->color, BlendType::kNormal);

	tex2D_->Draw(moveGaugeLeftState_->transform.matWorld_,Mat4x4::kIdentity, camera.GetViewOthographics()
		, moveGaugeLeftState_->textureID, moveGaugeLeftState_->color, BlendType::kNormal);

	tex2D_->Draw(moveGaugeCenterState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, moveGaugeCenterState_->textureID, moveGaugeCenterState_->color, BlendType::kNormal);
	
}

void ItemGauge::Debug(){
#ifdef _DEBUG


	ImGui::Begin("ゲージ関連");
	ImGui::DragFloat("ゲージ増加割合", &num_, 0.001f, 0.0f, 1.0f);
	Vector4 color = moveGaugeCenterState_->color;

	ImGui::ColorEdit4("ゲージ真ん中の色", color.data());
	ImGui::DragFloat3("縁の座標", gaugeState_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("縁の大きさ", gaugeState_->transform.scale.data(), 1.0f);
	ImGui::DragFloat3("ゲージ本体左の座標", moveGaugeLeftState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ゲージ本体右の座標", moveGaugeRightState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ゲージ本体中央の座標", moveGaugeCenterState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat2("ゲージ本体中央の大きさ", moveGaugeCenterState_->transform.scale.data(), 0.1f);
	ImGui::DragFloat2("現在の個数と前の個数", Vector2(static_cast<float>(beforeItemNum), static_cast<float>(afterItemNum)).data(), 1.0f);
	ImGui::End();
	moveGaugeCenterState_->color = color.GetColorRGBA();
#endif // _DEBUG
}

void ItemGauge::MoveGauge(){
	float& leftPos = moveGaugeLeftState_->transform.translate.x;
	float& rightPos = moveGaugeRightState_->transform.translate.x;

	rightPos = kGaugePosX_.x - (gaugePosLength_ * (num_));
	//rightPos = 305.0f;

	moveGaugeCenterState_->transform.translate.x = (leftPos + rightPos) * 0.5f;
	moveGaugeCenterState_->transform.scale.x = (kGaugeScale_ * (num_));
	
	moveGaugeCenterState_->transform.scale.y = 48.0f;
	
}
