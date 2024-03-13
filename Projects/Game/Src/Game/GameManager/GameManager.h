#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>
#include <list>

#include "../LambEngine/Input/Input.h"

#include "GameObject/GameObject.h"


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

	/// @brief 入力動作
	void InputAction();

private:

	GameObject *AddIvy();

private:
	Input *input_ = nullptr;
	std::list<std::unique_ptr<GameObject>> ivys_;

};