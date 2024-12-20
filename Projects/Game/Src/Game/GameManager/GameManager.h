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
	inline static const char *const kDwarfModelName = "Resources/Cube.obj";

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
	GameObject *AddFallingBlock(Vector2 centerPos, Vector2 size, Block::BlockType blockType, Vector2 velocity, Vector2 gravity, bool hasDamage = true, uint32_t blockDamage = 0);

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

	std::list<DwarfPick> PickUpObject(const POINTS localPos);

	void RandomDwarfSpawn();

	void RandomFallBlockSpawn();

	SoLib::VItem<"破壊時の停止時間", float> vBreakStopTime_{ 0.5f };

	SoLib::VItem<"開始してからブロックが沸くまでの時間", float> vFallBegin_{ 2.5f };
	SoLib::VItem<"落下するまでの間隔(sec)", float> vFallSpan_{ 2.5f };
	SoLib::VItem<"敵の沸く間隔(sec)", float> vSpawnSpan_{ 2.5f };

	SoLib::VItem<"クリアに必要なアイテムの数", int32_t> vClearItemCount_{ 150 };
	SoLib::VItem<"最大時間", int32_t> vMaxTime_{ 90 };

	SoLib::VItem<"ブロックの種類", int32_t> vBlockTypeCount_{ 4 };
	SoLib::VItem<"生成するブロックの高さ", int32_t> vStartBlockHeight_{ 3 };
	static inline SoLib::VItem<"落下するブロックの発生係数", int32_t> vFallPosCalc_{ 2 };

	static inline SoLib::VItem<"出てくるアイテムの間隔", float> vItemSpawnSpan_{ 0.25f };
	SoLib::VItem<"初期化の時の最低連結数", int32_t> vMinChainBlockCount_{ 1 };
	SoLib::VItem<"初期化の時の最大連結数", int32_t> vMaxChainBlockCount_{ 6 };

	SoLib::VItem<"下限となる結合数", int32_t> vLowerChainCount_{ 4 };
	SoLib::VItem<"下限以下と扱われるグループ数", int32_t> vLowerNeedCount_{ 4 };
	SoLib::VItem<"上限となる結合数", int32_t> vUpperChainCount_{ 6 };
	SoLib::VItem<"上限以上と扱われるグループ数", int32_t> vUpperNeedCount_{ 4 };

	/// @brief 調整項目
	inline static constexpr SoLib::VItemList vGameManagerItems_{ &GameManager::vBreakStopTime_, &GameManager::vFallBegin_, &GameManager::vFallSpan_, &GameManager::vSpawnSpan_, &GameManager::vClearItemCount_, &GameManager::vMaxTime_, &GameManager::vBlockTypeCount_, &GameManager::vStartBlockHeight_, &GameManager::vMinChainBlockCount_, &GameManager::vMaxChainBlockCount_, &GameManager::vLowerChainCount_, &GameManager::vLowerNeedCount_, &GameManager::vUpperChainCount_, &GameManager::vUpperNeedCount_ };
	inline static constexpr SoLib::VItemList vBlockMapItems_ = { &BlockMap::vCenterDiff_, &GameManager::vFallPosCalc_ };

	inline static constexpr SoLib::VItemList vItemStatus_ = { &vItemSpawnSpan_ };

	const auto &GetBreakTimer() const { return blockBreakTimer_; }

	GameObject *GetPlayer() { return player_.get(); }

	void SetGameUIManager(GameUIManager *pGameUIManager) {
		pGameUIManager_ = pGameUIManager;
	}
	/// @brief ブロック破壊時のアイテム追加
	void AddItem(const Vector2 globalPos, const Block::BlockType blockType, const uint32_t count = 1);

	void AddPoint();

	void RemovePoint(const int32_t count);

public:
	/// @brief 入力動作
	void InputAction();

	GameTimer *GetGameTimer() const { return gameTimer_.get(); }

	const int32_t GetClearItemCount() const { return vClearItemCount_.item; }

	const int32_t GetItemCount()const { return itemCount_; }

	const uint32_t GetItemSpawnCount() const { return itemSpawnCount_; }

	/// @brief 各アイテムごとの個数
	/// @param blockType アイテムのタイプ
	/// @return アイテムの個数
	const int32_t GetItemTypeCount(const Block::BlockType blockType) const {
		uint32_t index = static_cast<uint32_t>(blockType);
		index = std::clamp(index, 1u, static_cast<uint32_t>(Block::BlockType::kMax) - 1) - 1;
		return itemTypeCount_[index];
	}
	/// @brief 各アイテムが破棄された個数
	/// @param blockType アイテムのタイプ
	/// @return アイテムの個数
	const int32_t GetRemoveTypeCount(const Block::BlockType blockType) const {
		uint32_t index = static_cast<uint32_t>(blockType);
		index = std::clamp(index, 1u, static_cast<uint32_t>(Block::BlockType::kMax) - 1) - 1;
		return removeTypes_[index];
	}

	const auto &GetFallingBlocksPos() const {
		return fallingBlocksPos_;
	}

	void SetCamera(Camera *camera) {
		camera_ = camera;
	}

	const Camera *const GetCamera() const {
		return camera_.get();
	}

private:
	void BlockMapDropDown();

	void MargeDwarf();

	void ClearCheck();

	void PlayerMoveSafeArea();

	std::array<int32_t, 9u> LoadLevelData(int32_t levelIndex);

	void RandomStartBlockFill(const std::array<int32_t, 9u> &map, const int32_t blockTypeCount, const int32_t maxChainCount, const int32_t minChainCount = 1);

private:

#ifdef _DEBUG

	std::array<std::array<bool, BlockMap::kMapX>, BlockMap::kMapY> blockMapData_;

#endif // _DEBUG


	uint32_t pointTex_;
	SoLib::Time::DeltaTimer bonusPointDrawTimer_;

	Transform bonusTexTransform_;

	uint32_t itemSpawnCount_;

	std::array<int32_t, static_cast<uint32_t>(Block::BlockType::kMax) - 1> itemTypeCount_;
	std::array<int32_t, static_cast<uint32_t>(Block::BlockType::kMax) - 1> removeTypes_;

	std::list<std::pair<GameObject *, Vector2>> fallingBlocksPos_;

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

	Lamb::SafePtr<Camera> camera_;

	Audio *slimeDeath_ = nullptr;
	Audio *putBlock_ = nullptr;
};