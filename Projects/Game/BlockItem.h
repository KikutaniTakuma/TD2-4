#pragma once
#include"Game/Block/Block.h"
#include<Math/Mat4x4.h>
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"

class BlockItem{
public:
	BlockItem(const Vector2& startPos, const Vector2& endPos, const Block::BlockType& type);


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Mat4x4& cameraMat);


private:
	void MoveTexture();

	void SetControlPosition();

public:
	void SetIsMove(const bool flug) {
		isMove_ = flug;
	}

	const bool GetIsEnd()const {
		return isEndMove_;
	}

private:
	Texture2D* tex2D_ = nullptr;	

	std::unique_ptr<Tex2DState> texState_;

	std::array<Vector2, 4> controlPoints_;

	std::array<float, 3> moveSpeeds_;

	Vector2 secondPointPos_ = { 50.0f,100.0f };

	Vector2 thirdPointPos_ = { 100.0f,50.0f };

	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;


	//現在移動しているライン
	uint32_t linePass_ = 0;

	//アイテムが動くかどうか
	bool isMove_;
	//移動が終点にまで達していたら
	bool isEndMove_;
};

