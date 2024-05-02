#pragma once
#include "Game/Map/Map.h"
#include "Utils/SafePtr/SafePtr.h"

// ゲームマネージャ
class GameManager;	// シングルトン。前方宣言なのでSafePtrは使えない。

class GameEffectManager {
	friend GameManager;
public:
	GameEffectManager() = default;
	~GameEffectManager() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera &camera) const;

private:

	// ゲームマネージャの参照。
	GameManager *pGameManager_ = nullptr;

	Lamb::SafePtr<Map> pMap_ = nullptr;

	static Vector2 ToGrobal(const Vector2 localPos) noexcept { return Map::GetGlobalPos(localPos); }

	// ↓ 自動的に追加、破棄を行う。

	// 破壊されたブロックのローカル座標
	std::list<Vector2> blockBreakPos_;

	// 小人が死んだ時のローカル座標
	std::list<Vector2> dwarfDeadPos_;

	// 落下ブロックの予測
	std::list<Vector2> fallingBlocks_;

};