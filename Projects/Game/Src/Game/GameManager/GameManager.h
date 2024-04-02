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

class GameManager : public SoLib::Singleton<GameManager> {
private:

	friend SoLib::Singleton<GameManager>;
	GameManager() = default;
	GameManager(const GameManager &) = delete;
	GameManager &operator=(const GameManager &) = delete;
	~GameManager() = default;

public:

public:

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:

	bool Debug(const char *const str);

	// マップのデータを取得
	Map *GetMap() { return blockMap_.get(); }

public:

	/// @brief 入力動作
	void InputAction();

private:

private:
	// 入力マネージャ
	Input *input_ = nullptr;

	std::unique_ptr<GameObject> player_;

	std::unique_ptr<Map> blockMap_;

};