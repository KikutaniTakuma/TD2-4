#include "GameEffectManager.h"
#include "Game/GameManager/GameManager.h"

void GameEffectManager::Init()
{
	pGameManager_ = GameManager::GetInstance();
	pMap_ = pGameManager_->GetMap();
}

void GameEffectManager::Update([[maybe_unused]] float deltaTime)
{

}

void GameEffectManager::Draw([[maybe_unused]] const Camera &camera) const
{
}

void GameEffectManager::Clear()
{
	blockBreakPos_.clear();
	dwarfDeadPos_.clear();
	fallingBlock_ = { -1,-1 };
}
