#pragma once
#include "Game/Map/BlockMap.h"
#include "Utils/SafePtr/SafePtr.h"
#include <Drawers/Texture2D/Texture2D.h>

#include "Drawers/Particle/Particle.h"
#include <Game/Timer/Timer.h>

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

	void Clear();

private:

	// ゲームマネージャの参照。
	GameManager *pGameManager_ = nullptr;

	Lamb::SafePtr<BlockMap> pMap_ = nullptr;

	Texture2D *pSpriteDrawer = nullptr;

	uint32_t whiteTex_;

	static Vector2 ToGrobal(const Vector2 localPos) noexcept { return BlockMap::GetGlobalPos(localPos); }
	static Vector2 ToGrobal(const POINTS localPos) noexcept { return BlockMap::GetGlobalPos(localPos); }

	// ↓ 自動的に追加、破棄を行う。

	// 破壊されたブロックのローカル座標
	std::pair<Block::BlockType, BlockMap::BlockBitMap> blockBreakPos_;

	std::list<Vector2> margeDwarfPos_;

	// 小人が死んだ時のローカル座標
	std::list<Vector2> dwarfDeadPos_;

	// 落下ブロックの予測
	std::bitset<BlockMap::kMapX> fallingBlock_;	// -1なら無効

	std::vector<std::unique_ptr<Particle>> blockParticles_;
	std::array<std::unique_ptr<Particle>, 20> dwarfParticle_;
	//int32_t dwarfParticleIndex_ = 0;

	class Audio* blockBreak_ = nullptr;
};