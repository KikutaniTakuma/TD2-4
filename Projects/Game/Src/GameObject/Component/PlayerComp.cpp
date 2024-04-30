#include "PlayerComp.h"


void PlayerComp::Init()
{
	localPosX_ = {};
}

void PlayerComp::Start()
{
}

void PlayerComp::Update()
{
	transform_.translate = Map::GetGrobalPos(localPosX_, Map::kMapY);

}

void PlayerComp::SetMap(Map *map)
{
	// マップのポインタを代入
	if (map) {
		pMap_ = map;
	}
}

void PlayerComp::SetGameManager(GameManager *pGameManager)
{
	// ゲームマネージャのポインタを追加
	if (pGameManager) {
		pGameManager_ = pGameManager;
	}
}

void PlayerComp::MoveInput(int32_t xMove)
{
	localPosX_ += xMove;
	localPosX_ = std::clamp(localPosX_, 0, Map::kMapX - 1);
}

void PlayerComp::SpawnFallingBlock()
{

	// pGameManager_->AddFallingBlock()

}
