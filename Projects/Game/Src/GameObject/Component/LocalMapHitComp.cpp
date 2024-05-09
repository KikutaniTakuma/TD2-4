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
	Vector2 hitNormal = {};
	Vector2 velocity = pRigidbody_->GetVelocity();

	const Vector2 centor = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f;

	const Vector2 centorDown = centor + Vector2{0, pLocalBodyComp_->size_.y * -0.5f};

	const Vector2 leftDown = centor - pLocalBodyComp_->size_ * 0.5f;
	const Vector2 rightTop = centor + pLocalBodyComp_->size_ * 0.5f;

	[[maybe_unused]] const Vector2 leftTop = { leftDown.x, rightTop.y };
	[[maybe_unused]] const Vector2 rightDown = { rightTop.x, leftDown.y };

	// 左が画面外だったら
	if (leftDown.x < 0) {
		pLocalBodyComp_->localPos_.x = 0 - 0.5f + pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
		hitNormal.x = 1.f;
	}
	// 右が画面外だったら
	else if (rightDown.x >= static_cast<float>(BlockMap::kMapX)) {
		pLocalBodyComp_->localPos_.x = BlockMap::kMapX - 0.5f - pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
		hitNormal.x = -1.f;
	}
	// 下が画面外だったら
	if (rightDown.y < 0) {
		pLocalBodyComp_->localPos_.y = 0 - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
		velocity.y = 0;
		hitNormal.y = 1.f;
	}

	if (pBlockMap_->GetBlockType(leftDown) != Block::BlockType::kNone) {
		if (pBlockMap_->GetBlockType(leftDown + Vector2::kYIdentity) != Block::BlockType::kNone) {
			pLocalBodyComp_->localPos_.x = leftDown.x - 0.5f + pLocalBodyComp_->size_.x * 0.5f;
			velocity.x = 0;
			hitNormal.x = 1.f;
		}
	}

	if (pBlockMap_->GetBlockType(centorDown) != Block::BlockType::kNone) {
		pLocalBodyComp_->localPos_.y = std::ceil(centorDown.y) - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
		velocity.y = 0;
		hitNormal.y = 1.f;
	}


	// 接触した法線を返す
	hitNormal_ = hitNormal.Normalize();

	pRigidbody_->SetVelocity(velocity);


}
