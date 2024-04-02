#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>
#include <bitset>
#include <list>
#include <cstdint>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"
#include <Drawers/Model/Model.h>
#include <Game/CollisionManager/AABB/AABB.h>

class IvyComponent;

class GameManager : public SoLib::Singleton<GameManager> {
private:

	friend SoLib::Singleton<GameManager>;
	GameManager() = default;
	GameManager(const GameManager &) = delete;
	GameManager &operator=(const GameManager &) = delete;
	~GameManager() = default;

public:

	void Init();

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

public:

	bool Debug(const char *const str);

	void AddBox(AABB &&aabb);

public:

	/// @brief 入力動作
	void InputAction();

private:

	void BoxDraw(const Camera &camera) const;

	/// @brief AABBのデータをモデルに転送
	void TransferBoxData();

private:
	// 入力マネージャ
	Input *input_ = nullptr;
	std::list<std::unique_ptr<Model>> models_;
	std::list<AABB> aabbCollisions_;

	Vector3 playerPos_;

};