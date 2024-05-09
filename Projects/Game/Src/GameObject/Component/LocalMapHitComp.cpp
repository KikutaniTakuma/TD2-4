#include "LocalMapHitComp.h"
#include "Game/GameManager/GameManager.h"

void LocalMapHitComp::Init()
{
	// マップの参照を渡す
	pBlockMap_ = GameManager::GetInstance()->GetMap();

	// 体の判定を取得
	pRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();

}

void LocalMapHitComp::Update()
{
	Vector2 velocity = pRigidbody_->GetVelocity();

	const Vector2 centor = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f;

	const Vector2 leftDown = centor - pLocalBodyComp_->size_ * 0.5f;
	const Vector2 rightTop = centor + pLocalBodyComp_->size_ * 0.5f;

	[[maybe_unused]] const Vector2 leftTop = { leftDown.x, rightTop.y };
	[[maybe_unused]] const Vector2 rightDown = { rightTop.x, leftDown.y };

	// 左下が画面外だったら
	if (leftDown.x < 0) {
		pLocalBodyComp_->localPos_.x = 0 - 0.5f + pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
	}
	// 右上が画面外だったら
	else if (rightDown.x >= static_cast<float>(BlockMap::kMapX)) {
		pLocalBodyComp_->localPos_.x = BlockMap::kMapX - 0.5f - pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
	}

	pRigidbody_->SetVelocity(velocity);


	//pBlockMap_->GetBlockType(leftDown);
}
