#pragma once
#include"Game/Block/Block.h"
#include "Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include <SoLib_Timer.h>

class BlockItem {
public:
	BlockItem(const Vector2 &startPos, const Vector2 &endPos, const Block::BlockType &type, float startTime = 0);


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const float deltaTime);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Mat4x4 &cameraMat);


private:
	void MoveTexture(const float deltaTime);

	void SetControlPosition();

public:
	void SetIsMove(const bool flug) {
		isMove_ = flug;
	}

	const bool GetIsEnd()const {
		return isEndMove_;
	}

	const auto GetItemType() const { return itemType_; }

private:
	Texture2D *tex2D_ = nullptr;

	std::unique_ptr<Tex2DState> texState_;

	std::array<Vector2, 4> controlPoints_;

	std::array<float, 3> moveSpeeds_;

	Vector2 secondPointPos_ = { 1.0f,2.0f };

	Vector2 thirdPointPos_ = { 2.0f,1.0f };

	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;

	Block::BlockType itemType_;

	//現在移動しているライン
	uint32_t linePass_ = 0;

	// アイテムが動くまでの時間
	SoLib::Time::DeltaTimer startTimer_;

	// アイテムが動くかどうか
	bool isMove_;
	//移動が終点にまで達していたら
	bool isEndMove_;
};

