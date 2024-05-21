#pragma once
#include <Camera/Camera.h>
#include <Game/Timer/Timer.h>

class GameManager;

class GameUIManager {
public:
	GameUIManager() = default;
	~GameUIManager() = default;

	void Init(GameManager *pGameManager);

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

private:


	// タイマーの表示関数
	std::unique_ptr<GameTimerRender> gameTimerRender_;

	GameManager *pGameManager_;
};