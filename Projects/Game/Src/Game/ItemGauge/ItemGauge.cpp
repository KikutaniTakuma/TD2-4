#include "ItemGauge.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include <Drawers/DrawerManager.h>
#include<imgui.h>

void ItemGauge::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	gaugeFrameState_ = std::make_unique<Tex2DState>();
	gaugeFrameState_->color = 0xffffffff;
	gaugeFrameState_->transform.scale = { 780,72.0f };
	gaugeFrameState_->transform.translate = { -160.0f, kGaugeYPos };
	gaugeFrameState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeFrame.png");

	gaugeVoidState_ = std::make_unique<Tex2DState>();
	gaugeVoidState_->color = 0xffffffff;
	gaugeVoidState_->transform.scale = { 780.0f,72.0f };
	gaugeVoidState_->transform.translate = { -160.0f, kGaugeYPos };
	gaugeVoidState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeFrameVoid.png");

	moveGaugeLeftState_ = std::make_unique<Tex2DState>();
	moveGaugeLeftState_->color = gaugeColorBase_;
	moveGaugeLeftState_->transform.scale = { 36.0f,48.0f };
	moveGaugeLeftState_->transform.translate = { kGaugeLeftPosX_, kGaugeYPos };
	moveGaugeLeftState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSideWhite.png");

	moveGaugeRightState_ = std::make_unique<Tex2DState>();
	moveGaugeRightState_->color = gaugeColorBase_;
	moveGaugeRightState_->transform.scale = { 36.0f,48.0f };
	moveGaugeRightState_->transform.translate = { kGaugePosX_.x, kGaugeYPos };
	moveGaugeRightState_->uvTransform.scale = { -1.0f,1.0f };
	moveGaugeRightState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSideWhite.png");

	moveGaugeCenterState_ = std::make_unique<Tex2DState>();
	moveGaugeCenterState_->color = gaugeColorBase_;
	moveGaugeCenterState_->transform.scale = { 0.0f,47.0f };
	moveGaugeCenterState_->transform.translate = { -492.0f, kGaugeYPos-1.0f };
	moveGaugeCenterState_->textureID = TextureManager::GetInstance()->GetWhiteTex();

	moveGaugeReduction_ = std::make_unique<Tex2DState>();
	moveGaugeReduction_->color = 0xff0000ff;
	moveGaugeReduction_->transform.scale = { 0.0f,46.0f };
	moveGaugeReduction_->transform.translate = { 0.0f, 315.0f ,-5.0f};
	moveGaugeReduction_->textureID = TextureManager::GetInstance()->GetWhiteTex();

	moveGaugeReductionRight_ = std::make_unique<Tex2DState>();
	moveGaugeReductionRight_->color = 0xff0000ff;
	moveGaugeReductionRight_->transform.scale = { 36.0f,48.0f };
	moveGaugeReductionRight_->transform.translate = { 200.0f, 315.0f };
	moveGaugeReductionRight_->uvTransform.scale = { -1.0f,1.0f };
	moveGaugeReductionRight_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Gauge/potGaugeSideWhite.png");

	gaugePosLength_ = kGaugePosX_.x - kGaugePosX_.y;
	gaugePosCenterLength_ = kGaugeCenterPosX_.x - kGaugeCenterPosX_.y;

	isItemGaugeMax_ = false;
}

void ItemGauge::Update(const int32_t& nowCount, const int32_t& maxCount){
	isItemReduction_ = false;
	if (afterItemNum_ != nowCount) {
		if (afterItemNum_ > nowCount) {
			isItemReduction_ = true;
			isItemReductionNow_ = true;
			isItemIncreaseNow_ = false;
		}
		else {
			isItemReductionNow_ = false;
			isItemIncreaseNow_ = true;
			beforeIncreaseGaugeCenterPos_ = moveGaugeCenterState_->transform.translate.x;
			beforeIncreaseGaugeRightPos_ = moveGaugeRightState_->transform.translate.x;
			beforeIncreaseGaugeCenterScale_ = moveGaugeCenterState_->transform.scale.x;
		}

		colorTime_ = 0;
		beforeItemNum_ = afterItemNum_;
		afterItemNum_ = nowCount;
	}
	//maxCount;
	num_ = static_cast<float>(nowCount) / static_cast<float>(maxCount);

	if (num_ >= 1.0f) {
		num_ = 1.0f;
		
	}

	MoveGauge();

	gaugeFrameState_->transform.CalcMatrix();
	gaugeVoidState_->transform.CalcMatrix();
	moveGaugeLeftState_->transform.CalcMatrix();
	moveGaugeRightState_->transform.CalcMatrix();
	moveGaugeRightState_->uvTransform.CalcMatrix();
	moveGaugeCenterState_->transform.CalcMatrix();
	moveGaugeReduction_->transform.CalcMatrix();
	moveGaugeReductionRight_->transform.CalcMatrix();
	moveGaugeReductionRight_->uvTransform.CalcMatrix();

}

void ItemGauge::Draw(const Camera& camera) const{

	tex2D_->Draw(gaugeFrameState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, gaugeFrameState_->textureID, gaugeFrameState_->color, BlendType::kNormal);
	

	tex2D_->Draw(moveGaugeRightState_->transform.matWorld_,moveGaugeRightState_->uvTransform.matWorld_, camera.GetViewOthographics()
		, moveGaugeRightState_->textureID, gaugeColorBase_, BlendType::kNormal);

	tex2D_->Draw(moveGaugeLeftState_->transform.matWorld_,Mat4x4::kIdentity, camera.GetViewOthographics()
		, moveGaugeLeftState_->textureID, gaugeColorBase_, BlendType::kNormal);

	tex2D_->Draw(moveGaugeCenterState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, moveGaugeCenterState_->textureID, gaugeColorBase_, BlendType::kNormal);
	if (isItemReductionNow_) {

		tex2D_->Draw(moveGaugeReduction_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
			, moveGaugeReduction_->textureID, reductionColor_, BlendType::kNormal);

		tex2D_->Draw(moveGaugeReductionRight_->transform.matWorld_, moveGaugeReductionRight_->uvTransform.matWorld_, camera.GetViewOthographics()
			, moveGaugeReductionRight_->textureID, reductionColor_, BlendType::kNormal);
	}
	tex2D_->Draw(gaugeVoidState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, gaugeVoidState_->textureID, gaugeVoidState_->color, BlendType::kNormal);
}

void ItemGauge::Debug(){
#ifdef _DEBUG


	ImGui::Begin("ゲージ関連");
	ImGui::DragFloat("ゲージ増加割合", &num_, 0.001f, 0.0f, 1.0f);
	Vector4 color = moveGaugeCenterState_->color;

	ImGui::ColorEdit4("ゲージ真ん中の色", color.data());
	ImGui::DragFloat3("ゲージ本体左の座標", moveGaugeLeftState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ゲージ本体右の座標", moveGaugeRightState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat3("ゲージ本体中央の座標", moveGaugeCenterState_->transform.translate.data(), 0.1f);
	ImGui::DragFloat2("ゲージ本体中央の大きさ", moveGaugeCenterState_->transform.scale.data(), 0.1f);
	ImGui::DragFloat2("現在の個数と前の個数", Vector2(static_cast<float>(beforeGaugeCenterRight_), static_cast<float>(afterGaugeCenterRight_)).data(), 1.0f);
	//ImGui::DragFloat2("真ん中の座標ｘと大きさ", Vector2(reductionGaugePos_,reductionGaugeScale_).data(), 1.0f);
	ImGui::DragFloat4("真ん中の情報(前：後)", Vector4(beforeIncreaseGaugeCenterPos_, beforeIncreaseGaugeCenterScale_,
		afterIncreaseGaugeCenterPos_, afterIncreaseGaugeCenterScale_).data(), 1.0f);

	ImGui::End();
	moveGaugeCenterState_->color = color.GetColorRGBA();
#endif // _DEBUG
}

void ItemGauge::MoveGauge(){
	float& leftPos = moveGaugeLeftState_->transform.translate.x;
	float& rightPos = moveGaugeRightState_->transform.translate.x;
	//rightPos = 305.0f;
	auto& centerTrans = moveGaugeCenterState_->transform;
	const float rightPosAfter = kGaugePosX_.x - (gaugePosLength_ * (num_));

	if (rightPos >= kGaugePosX_.y) {
		isItemGaugeMax_ = true;
	}

	float multiNum = 2.0f;

	afterIncreaseGaugeRightPos_ = rightPosAfter;
	afterIncreaseGaugeCenterPos_ = (leftPos + rightPosAfter) * 0.5f;
	afterIncreaseGaugeCenterScale_ = (kGaugeScale_ * (num_) * 1.01f);
	

	const auto& gaugeRightPos_ = ((leftPos + rightPosAfter) * 0.5f) + ((kGaugeScale_ * num_) * 0.5f);

	auto& tex = moveGaugeReduction_->transform;

	if (afterGaugeCenterRight_ != gaugeRightPos_) {
		beforeGaugeCenterRight_ = afterGaugeCenterRight_;
		afterGaugeCenterRight_ = gaugeRightPos_;

		reductionGaugePos_ = (beforeGaugeCenterRight_ + afterGaugeCenterRight_) * 0.5f;

		reductionGaugeScale_ = (beforeGaugeCenterRight_ - afterGaugeCenterRight_);

		tex.translate.x = reductionGaugePos_;

		tex.scale.x = reductionGaugeScale_;

		moveGaugeReductionRight_->transform.translate.x = tex.translate.x + (tex.scale.x * 0.5f) + (moveGaugeReductionRight_->transform.scale.x * 0.25f);

	}

	if (isItemReductionNow_) {
		rightPos = rightPosAfter;
		centerTrans.translate.x = (leftPos + rightPosAfter) * 0.5f;
		centerTrans.scale.x = (kGaugeScale_ * (num_));
		
		
		if (tex.scale.x <= 1.2f) {
			tex.scale.x = 0;
			isItemReductionNow_ = false;
		}
		else {
			tex.scale.x -= reductionGaugeScale_ * Lamb::DeltaTime() * multiNum;
			tex.translate.x -= (reductionGaugePos_ - afterGaugeCenterRight_) * Lamb::DeltaTime() * multiNum;

			colorTime_++;
			if (colorTime_ == kChangeColorTime_) {
				if (reductionColor_ == RedWhite_) {
					reductionColor_ = Red_;
				}
				else {
					reductionColor_ = RedWhite_;
				}
				colorTime_ = 0;
			}
			

		}

		moveGaugeReductionRight_->transform.translate.x = tex.translate.x + (tex.scale.x * 0.5f) + (moveGaugeReductionRight_->transform.scale.x * 0.25f);
		colorTime_++;
	}

	if (isItemIncreaseNow_) {
		

		if (centerTrans.scale.x >= afterIncreaseGaugeCenterScale_) {
			centerTrans.scale.x = afterIncreaseGaugeCenterScale_;
			isItemIncreaseNow_ = false;
			gaugeColorBase_ = gaugeColor_;
		}
		else {
			centerTrans.scale.x += (afterIncreaseGaugeCenterScale_ - beforeIncreaseGaugeCenterScale_) * Lamb::DeltaTime() * multiNum;

			float addPosCenter = (afterIncreaseGaugeCenterPos_ - beforeIncreaseGaugeCenterPos_);
			if (addPosCenter < 0) {
				addPosCenter *= -1;
			}

			centerTrans.translate.x += (addPosCenter) * Lamb::DeltaTime() * multiNum;

			float addPosRight = (afterIncreaseGaugeRightPos_ - beforeIncreaseGaugeRightPos_);
			if (addPosRight < 0) {
				addPosRight *= -1;
			}

			rightPos += (addPosRight) * Lamb::DeltaTime() * multiNum;
			
			colorTime_++;
			if (colorTime_ == kChangeColorTime_) {
				if (gaugeColorBase_ == gaugeColorChange_) {
					gaugeColorBase_ = gaugeColor_;
				}
				else {
					gaugeColorBase_ = gaugeColorChange_;
				}
				colorTime_ = 0;
			}

		}

		
	}

	
	/*moveGaugeCenterState_->transform.scale.y = 48.0f;*/
	
}
