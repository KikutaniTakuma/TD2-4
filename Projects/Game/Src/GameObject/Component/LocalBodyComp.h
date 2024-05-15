#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"

#include "Game/Map/BlockMap.h"
#include "Game/GameManager/GameManager.h"

class LocalBodyComp : public IComponent
{
public:
	using IComponent::IComponent;
	~LocalBodyComp() = default;

	/// @brief マップ上のローカル座標を取得
	/// @return マップのIndex
	POINTS GetMapPos() const { return {.x = static_cast<int16_t>(localPos_.x + 0.5f), .y = static_cast<int16_t>(localPos_.y + 0.5f)}; }

	// マップの参照
	inline static Lamb::SafePtr<BlockMap> pMap_;

	// ゲームマネージャへの参照
	inline static Lamb::SafePtr<GameManager> pGameManager_;

	/// @brief ローカル座標をグローバル座標に変換する
	/// @return グローバル座標
	Vector2 GetGlobalPos() const;

	/// @brief データをTransformに転送する
	void TransfarData();

	// ローカル座標
	Vector2 localPos_;
	// ブロックの大きさ
	Vector2 size_;

	float drawScale_ = 0.5f;

private:

};