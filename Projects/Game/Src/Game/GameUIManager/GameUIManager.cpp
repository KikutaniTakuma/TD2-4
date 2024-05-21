#include "GameUIManager.h"
#include "Game/GameManager/GameManager.h"

void GameUIManager::Init(GameManager *pGameManager)
{
	pGameManager_ = pGameManager;

	gameTimerRender_ = std::make_unique<GameTimerRender>();
	gameTimerRender_->Init(pGameManager_->GetGameTimer());
}

void GameUIManager::Update([[maybe_unused]] const float deltaTime)
{
	gameTimerRender_->Update(deltaTime);
}

void GameUIManager::Draw([[maybe_unused]] const Camera &camera) const
{
	gameTimerRender_->Draw(camera);
}
