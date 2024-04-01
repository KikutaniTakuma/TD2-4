#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>
#include <bitset>
#include <list>
#include <cstdint>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"

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

public:

	/// @brief 入力動作
	void InputAction();

private:
private:
	// 入力マネージャ
	Input *input_ = nullptr;
};