#pragma once
#include <cstdint>
#include <memory>

#include "Camera/Camera.h"
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SoLib/Containers/VItem.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Utils/Flg/Flg.h"


class ItemGauge{
public:
	ItemGauge() = default;
	~ItemGauge() = default;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const int32_t& nowCount, const int32_t& maxCount);


	void Draw(const Camera& camera)const;

	/// <summary>
	/// imgui
	/// </summary>
	void Debug();

	const Lamb::Flg& GetItemReduction()const {
		return isItemReduction_;
	}

	const Lamb::Flg& GetItemGaugeMax()const {
		return isItemGaugeMax_;
	}

	Vector3 GetGaugeRightPos()const {
		return moveGaugeRightState_->transform.translate;
	}

private:
	/// <summary>
	/// 入手したアイテムに応じてテクスチャをいじる
	/// </summary>
	void MoveGauge();

private:
	Texture2D* tex2D_;
	//始点終点
	const Vector2 kGaugePosX_ = { -479.0f,195.0f };
	const float kGaugeLeftPosX_ = -505.0f;
	float gaugePosLength_ = 659.0f;

	uint32_t gaugeColorBase_ = 0x5ea632ff;

	const uint32_t gaugeColor_ = 0x5ea632ff;

	const uint32_t gaugeColorChange_ = 0x7fc854ff;

	//減っているときの色
	uint32_t reductionColor_ = 0xff0000ff;

	const uint32_t RedWhite_ = 0xff9999ff;

	const uint32_t Red_ = 0xff0000ff;

	//フレーム刻みで変化させる
	const uint32_t kChangeColorTime_ = 3;
	uint32_t colorTime_ = 0;
	//現在のアイテムの個数
	int32_t beforeItemNum_;
	int32_t afterItemNum_;

	//前のフレームのゲージの真ん中の右端
	float beforeGaugeCenterRight_;
	//後のフレームのゲージの真ん中の右端
	float afterGaugeCenterRight_;
	//減った後のゲージのポジション
	float reductionGaugePos_;
	//減った後のゲージのスケール
	float reductionGaugeScale_;

	//アイテムが減少したときtrue
	Lamb::Flg isItemReduction_ = false;

	Lamb::Flg isItemReductionNow_ = false;

	Lamb::Flg isItemGaugeMax_ = false;


	//アイテムが増加したときにtrue
	Lamb::Flg isItemIncreaseNow_ = false;
	//増加するときのやつ
	//前のフレームの真ん中のポジション
	float beforeIncreaseGaugeCenterPos_;
	//後のフレームの真ん中のポジション
	float afterIncreaseGaugeCenterPos_;

	//前のフレームの右のポジション
	float beforeIncreaseGaugeRightPos_;
	//後のフレームの右のポジション
	float afterIncreaseGaugeRightPos_;

	//前のフレームの真ん中のスケール
	float beforeIncreaseGaugeCenterScale_;
	//後のフレームの真ん中のスケール
	float afterIncreaseGaugeCenterScale_;

	//右から左の設定
	//const Vector2 kGaugePosX_ = { 283.0f,-338.0f };

	//始点終点
	const Vector2 kGaugeCenterPosX_ = { 320.0f,-38.0f };
	float gaugePosCenterLength_ = 358.0f;
	const float kGaugeScale_ = 680.0f;

	const float kGaugeYPos = 315.0f;

	float num_ = 0.0f;
	//外側の縁
	std::unique_ptr<Tex2DState> gaugeFrameState_;

	//外側の縁
	std::unique_ptr<Tex2DState> gaugeVoidState_;
	//内側の緑色のやつ
	std::unique_ptr<Tex2DState> moveGaugeLeftState_;

	//内側の緑色のやつ
	std::unique_ptr<Tex2DState> moveGaugeRightState_;
	//真ん中のやつ
	std::unique_ptr<Tex2DState> moveGaugeCenterState_;
	
	std::unique_ptr<Tex2DState> moveGaugeReduction_;

	std::unique_ptr<Tex2DState> moveGaugeReductionRight_;

};

