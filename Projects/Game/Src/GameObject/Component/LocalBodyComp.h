#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"

#include "Game/Map/Map.h"
#include "Game/GameManager/GameManager.h"

class LocalBodyComp : public IComponent
{
public:
	using IComponent::IComponent;
	~LocalBodyComp() = default;

	// マップの参照
	inline static Lamb::SafePtr<Map> pMap_;

	// ゲームマネージャへの参照
	inline static Lamb::SafePtr<GameManager> pGameManager_;

	/// @brief ローカル座標をグローバル座標に変換する
	/// @return グローバル座標
	Vector2 LocalToGlobal() const;

	// ローカル座標
	Vector2 localPos_;
	// ブロックの大きさ
	Vector2 size_;

private:

};