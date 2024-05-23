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
#include <Game/Timer/Timer.h>
#include <Game/GameUIManager/GameUIManager.h>
#include"Game/Item/BlockItem.h"

class GameScene;

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

	using DwarfPick = std::pair<Vector2, std::unique_ptr<GameObject>>;

public:
	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

	void SetUIManager(GameUIManager *uiManager) { pGameUIManager_ = uiManager; }

	void LoadGlobalVariant(const uint32_t stageIndex = 0);

	void SaveGlobalVariant(const uint32_t stageIndex = 0) const;

public:
	bool Debug(const char *const str);

	void SetGameScene(GameScene *gameScene) { pGameScene_ = gameScene; }

	// マップのデータを取得
	BlockMap *GetMap() { return blockMap_.get(); }

	GameObject *AddPlayerBullet(Vector2 centerPos, Vector2 velocity);

	GameObject *AddEnemyBullet(Vector2 centerPos, Vector2 velocity);

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

	GameObject *AddDarkDwarf(Vector2 centerPos);

	GameObject *AddDwarf(std::unique_ptr<GameObject> dwarf);

	/// @brief 指定した座標のブロックを持ち上げる
	/// @param localPos 指定先
	/// @param hasBlockWeight すでに持っているブロックの重さ
	/// @param maxWeight 持つことのできる上限値
	/// @param isPowerful 上にブロックがあっても持ち上げるか
	/// @return [ 持ちあげたブロック, ブロックの中心座標 ]
	// std::pair<PickUpBlockData, Vector2> PickUp(Vector2 localPos, int hasBlockWeight, int maxWeight = 6, bool isPowerful = false);

	GameEffectManager *GetGameEffect() { return gameEffectManager_.get(); }

	std::unordered_set<POINTS> GetDwarfPos() const;

	BlockMap::BlockBitMap &&BreakChainBlocks(POINTS localPos);

	//弾が当たったときに当たった個所からつながっているブロックを検索して色を変化させる
	BlockMap::BlockBitMap &&HitChainBlocks(POINTS localPos);

	std::list<DwarfPick> PickUpBlockSideObject(const POINTS localPos);

	void RandomDwarfSpawn();

	void RandomFallBlockSpawn();

	SoLib::VItem<"破壊時の停止時間", float> vBreakStopTime_{ 0.5f };

	SoLib::VItem<"落下するまでの間隔(sec)", float> vFallSpan_{ 2.5f };
	SoLib::VItem<"敵の沸く間隔(sec)", float> vSpawnSpan_{ 2.5f };

	SoLib::VItem<"クリアに必要なアイテムの数", int32_t> vClearItemCount_{ 150 };
	SoLib::VItem<"最大時間", int32_t> vMaxTime_{ 90 };

	SoLib::VItem<"ブロックの種類", int32_t> vBlockTypeCount_{ 4 };
	SoLib::VItem<"生成するブロックの高さ", int32_t> vStartBlockHeight_{ 3 };

	static inline SoLib::VItem<"出てくるアイテムの間隔", float> vItemSpawnSpan_{ 0.25f };

	/// @brief 調整項目
	inline static constexpr SoLib::VItemList vGameManagerItems_{ &GameManager::vBreakStopTime_, &GameManager::vFallSpan_, &GameManager::vSpawnSpan_, &GameManager::vClearItemCount_, &GameManager::vMaxTime_, &GameManager::vBlockTypeCount_, &GameManager::vStartBlockHeight_ };
	inline static constexpr SoLib::VItemList vBlockMapItems_ = { &BlockMap::vCenterDiff_ };

	inline static constexpr SoLib::VItemList vItemStatus_ = { &vItemSpawnSpan_ };

	const auto &GetBreakTimer() const { return blockBreakTimer_; }

	GameObject *GetPlayer() { return player_.get(); }

	/// @brief ブロック破壊時のアイテム追加
	void AddItem(const Vector2 globalPos, const Block::BlockType blockType, const uint32_t count = 1);

	void AddPoint();

public:
	/// @brief 入力動作
	void InputAction();

	GameTimer *GetGameTimer() const { return gameTimer_.get(); }

	const int32_t GetClearItemCount() const { return vClearItemCount_.item; }

	const int32_t GetItemCount()const { return itemCount_; }

	/// @brief 各アイテムごとの個数
	/// @param blockType アイテムのタイプ
	/// @return アイテムの個数
	const uint32_t GetItemTypeCount(const Block::BlockType blockType) const {
		uint32_t index = static_cast<uint32_t>(blockType);
		index = std::clamp(index, 1u, static_cast<uint32_t>(Block::BlockType::kMax) - 1) - 1;
		return itemTypeCount_[index];
	}

private:
	void BlockMapDropDown();

	void MargeDwarf();

	void ClearCheck();

private:

	std::array<uint32_t, 4u> itemTypeCount_;

	std::list<std::unique_ptr<BlockItem>> itemList_;

	int32_t itemCount_;

	SoLib::Time::DeltaTimer blockBreakTimer_;

	SoLib::Time::DeltaTimer dwarfSpawnTimer_;
	SoLib::Time::DeltaTimer fallBlockSpawnTimer_;
	// 入力マネージャ
	Input *input_ = nullptr;

	std::unique_ptr<GameTimer> gameTimer_;

	std::unique_ptr<BlockGauge> blockGauge_ = nullptr;

	//std::unique_ptr<GameObject> spawner_;

	std::unique_ptr<GameObject> player_;

	std::unique_ptr<BlockMap> blockMap_;

	std::list<std::unique_ptr<GameObject>> fallingBlocks_;

	// ダメージ判定のリスト
	std::list<DamageArea> damageAreaList_;

	// 小人のリスト
	std::list<std::unique_ptr<GameObject>> dwarfList_;

	// プレイヤが撃った弾のリスト
	std::list<std::unique_ptr<GameObject>> plBulletList_;

	std::list<std::unique_ptr<GameObject>> enemyBulletList_;

	// プレイヤが撃った弾のリスト
	std::list<std::unique_ptr<GameObject>> darkDwarfList_;

	std::unique_ptr<GameEffectManager> gameEffectManager_ = nullptr;
	Lamb::SafePtr<GameUIManager> pGameUIManager_ = nullptr;

	GameScene *pGameScene_ = nullptr;
};