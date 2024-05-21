#include "GameUIManager.h"
#include "Game/GameManager/GameManager.h"

void GameUIManager::Init(GameManager *pGameManager)
{
	pGameManager_ = pGameManager;

	gameTimerRender_ = std::make_unique<GameTimerRender>();
	gameTimerRender_->Init(pGameManager_->GetGameTimer());

	itemGauge_ = std::make_unique<ItemGauge>();
	itemGauge_->Initialize();
}

void GameUIManager::Update([[maybe_unused]] const float deltaTime)
{
	gameTimerRender_->Update(deltaTime);
	itemGauge_->Update(pGameManager_->GetItemCount(), pGameManager_->GetClearItemCount());
	itemGauge_->Debug();
}

void GameUIManager::Draw([[maybe_unused]] const Camera &camera) const
{
	gameTimerRender_->Draw(camera);
	itemGauge_->Draw(camera);
}
