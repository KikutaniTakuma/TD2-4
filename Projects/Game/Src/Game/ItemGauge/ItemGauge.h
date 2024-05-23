#pragma once
#include <cstdint>
#include <memory>

#include "Camera/Camera.h"
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SoLib/Containers/VItem.h"
#include "Drawers/Texture2D/Texture2D.h"


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

private:
	/// <summary>
	/// 入手したアイテムに応じてテクスチャをいじる
	/// </summary>
	void MoveGauge();

private:
	Texture2D* tex2D_;
	//始点終点
	const Vector2 kGaugePosX_ = { -315.0f,305.0f };
	float gaugePosLength_ = 659.0f;

	//右から左の設定
	//const Vector2 kGaugePosX_ = { 283.0f,-338.0f };

	//始点終点
	const Vector2 kGaugeCenterPosX_ = { 320.0f,-38.0f };
	float gaugePosCenterLength_ = 358.0f;
	const float kGaugeScale_ = 620.0f;

	float num_ = 0.0f;
	//外側の縁
	std::unique_ptr<Tex2DState> gaugeState_;
	//内側の緑色のやつ
	std::unique_ptr<Tex2DState> moveGaugeLeftState_;

	//内側の緑色のやつ
	std::unique_ptr<Tex2DState> moveGaugeRightState_;

	std::unique_ptr<Tex2DState> moveGaugeCenterState_;

};

