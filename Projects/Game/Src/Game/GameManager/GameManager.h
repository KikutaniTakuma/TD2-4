#pragma once
#include "../SoLib/Containers/Array2D.h"
#include "../SoLib/Containers/Singleton.h"

#include <bitset>
#include <cstdint>
#include <list>
#include <memory>

#include "../LambEngine/Input/Input.h"
#include "Game/BlockGauge.h"
#include "Game/Map/BlockMap.h"

#include "GameObject/GameObject.h"
#include <Drawers/Model/Model.h>
#include <Game/CollisionManager/AABB/AABB.h>
#include <Game/GameEffectManager/GameEffectManager.h>

// ダメージ判定
struct DamageArea
{
	Vector2 centerPos_;
	Vector2 size_;
};

class GameManager : public SoLib::Singleton<GameManager>
{
private:
	friend SoLib::Singleton<GameManager>;
	GameManager() = default;
	GameManager(const GameManager &) = delete;
	GameManager &operator=(const GameManager &) = delete;
	~GameManager() = default;

public:
	inline static const char *kDwarfModelName = "Resources/Cube.obj";

public:
	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:
	bool Debug(const char *const str);

	// マップのデータを取得
	BlockMap *GetMap() { return blockMap_.get(); }

	GameObject *AddPlayerBullet(Vector2 centerPos, Vector2 velocity);

	/// <summary>
	/// 落下ブロックを追加する
	/// </summary>
	/// <param name="centerPos">中心座標</param>
	/// <param name="size">直径</param>
	/// <param name="velocity">瞬間加速</param>
	/// <param name="gravity">定期加速</param>
	GameObject *AddFallingBlock(Vector2 centerPos, Vector2 size, Block::BlockType blockType, Vector2 velocity, Vector2 gravity);

	/// @brief ブロックが接地した時の処理
	/// @param centerPos 中心座標
	/// @param size 直径
	/// @param hasDamage ダメージがあるか
	void LandBlock(Vector2 centerPos, Vector2 size, bool hasDamage);

	GameObject *AddDwarf(Vector2 centerPos);

	/// @brief 指定した座標のブロックを持ち上げる
	/// @param localPos 指定先
	/// @param hasBlockWeight すでに持っているブロックの重さ
	/// @param maxWeight 持つことのできる上限値
	/// @param isPowerful 上にブロックがあっても持ち上げるか
	/// @return [ 持ちあげたブロック, ブロックの中心座標 ]
	// std::pair<PickUpBlockData, Vector2> PickUp(Vector2 localPos, int hasBlockWeight, int maxWeight = 6, bool isPowerful = false);

	GameEffectManager *GetGameEffect() { return gameEffectManager_.get(); }

	std::unordered_set<POINTS> GetDwarfPos() const;

public:
	/// @brief 入力動作
	void InputAction();

private:
	void BlockMapDropDown();

	// void BreakEnemyHouse(int32_t facing, Map::HouseInfo enemyHouse)
	//{
	//	static const Vector2  kTowerBaseThrowSpeed_ = { 0.5f, 2.f };
	//	static const Vector2 kTowerMultipleSpeed_ = { 1.5f, 1.f };
	//	static const Vector2 kTowerGravityPower_ = { 0.f, -15.f };

	//	if (facing != 0)
	//	{
	//		blockMap_->ProcessEnemyHouseBlocks([facing, &enemyHouse, this](int32_t y, int32_t x)
	//			{
	//				Vector2 blockFinder = { static_cast<float>(enemyHouse.xPos_ + x), static_cast<float>(y) };
	//				if (blockMap_->GetBoxType(blockFinder) == Block::BlockType::kEnemyBlock)
	//				{
	//					auto [pickup, center] = PickUp(blockFinder, 0, 100, true);
	//					// 空っぽだったら飛ばす
	//					if (pickup.GetWeight() == 0) { return; }

	//					AddFallingBlock(center, pickup.size_, true, Vector2{ kTowerBaseThrowSpeed_.x + facing * blockFinder.y * kTowerMultipleSpeed_.x, kTowerBaseThrowSpeed_.y + blockFinder.y * kTowerMultipleSpeed_.y }, kTowerGravityPower_);
	//				}
	//			});
	//	}
	//}

private:
	// 入力マネージャ
	Input *input_ = nullptr;

	std::unique_ptr<BlockGauge> blockGauge_ = nullptr;

	std::unique_ptr<GameObject> spawner_;

	std::unique_ptr<GameObject> player_;

	std::unique_ptr<BlockMap> blockMap_;

	std::list<std::unique_ptr<GameObject>> fallingBlocks_;

	// ダメージ判定のリスト
	std::list<DamageArea> damageAreaList_;

	// 小人のリスト
	std::list<std::unique_ptr<GameObject>> dwarfList_;

	// プレイヤが撃った弾のリスト
	std::list<std::unique_ptr<GameObject>> plBulletList_;

	std::unique_ptr<GameEffectManager> gameEffectManager_ = nullptr;
};