#pragma once
#include "../SoLib/Containers/Singleton.h"

#include <memory>

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

private:

	std::unique_ptr<GameObject> mainIvy_ = nullptr;


};