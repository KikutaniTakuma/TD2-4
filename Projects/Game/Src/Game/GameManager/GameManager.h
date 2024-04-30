#pragma once
#include "../SoLib/Containers/Singleton.h"
#include "../SoLib/Containers/Array2D.h"

#include <memory>
#include <bitset>
#include <list>
#include <cstdint>

#include "../LambEngine/Input/Input.h"
#include "Game/Map/Map.h"

#include "GameObject/GameObject.h"
#include <Drawers/Model/Model.h>
#include <Game/CollisionManager/AABB/AABB.h>

// ダメージ判定
struct DamageArea {
	Vector2 centerPos_;
	Vector2 size_;
};


class GameManager : public SoLib::Singleton<GameManager> {
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
	Map *GetMap() { return blockMap_.get(); }

	/// <summary>
	/// 落下ブロックを追加する
	/// </summary>
	/// <param name="centerPos">中心座標</param>
	/// <param name="size">直径</param>
	/// <param name="velocity">瞬間加速</param>
	/// <param name="gravity">定期加速</param>
	GameObject *AddFallingBlock(Vector2 centerPos, Vector2 size, bool hasDamage, Vector2 velocity, Vector2 gravity);

	/// @brief ブロックが接地した時の処理
	/// @param centerPos 中心座標
	/// @param size 直径
	/// @param hasDamage ダメージがあるか
	void LandBlock(Vector2 centerPos, Vector2 size, bool hasDamage);

	GameObject *AddDwarf(Vector2 centerPos);

public:

	/// @brief 入力動作
	void InputAction();

private:

private:
	// 入力マネージャ
	Input *input_ = nullptr;

	std::unique_ptr<GameObject> player_;

	std::unique_ptr<Map> blockMap_;

	std::list<std::unique_ptr<GameObject>> fallingBlocks_;

	// ダメージ判定のリスト
	std::list<DamageArea> damageAreaList_;

	// 小人のリスト
	std::list<std::unique_ptr<GameObject>> dwarfList_;

};