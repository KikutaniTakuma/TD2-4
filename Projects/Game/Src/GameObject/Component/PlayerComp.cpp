#include "PlayerComp.h"


void PlayerComp::Init()
{
	pLocalPosComp_ = object_.AddComponent<LocalBodyComp>();
}

void PlayerComp::Start()
{
	pLocalPosComp_->localPos_ = Vector2{ 0,static_cast<float>(Map::kMapY) };
}

void PlayerComp::Update()
{
	transform_.translate = pLocalPosComp_->LocalToGlobal();

}

void PlayerComp::MoveInput(int32_t xMove)
{
	pLocalPosComp_->localPos_.x += xMove;
	pLocalPosComp_->localPos_.x = std::clamp(pLocalPosComp_->localPos_.x, 0.f, static_cast<float>(Map::kMapX - 1));
}

void PlayerComp::SpawnFallingBlock() const
{

	// 落下ブロックの実体の追加
	pLocalPosComp_->pGameManager_->AddFallingBlock(pLocalPosComp_->localPos_ - Vector2::kYIdentity, Vector2::kIdentity, false, Vector2::kYIdentity * -20, Vector2::kZero);

}
