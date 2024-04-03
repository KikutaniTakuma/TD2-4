#include "OnBlockMoveComp.h"
#include "Game/GameManager/GameManager.h"

void OnBlockMoveComp::Init()
{
	pMap_ = GameManager::GetInstance()->GetMap();
}

void OnBlockMoveComp::Update()
{
	// 着地している場合
	if (CheckLanding()) {
		// 通常の移動ができるか
		if (CheckNormalMove()) {

			// 移動
			onStagePos_ += moveDir_;
			// 移動量を0に
			moveDir_ = {};

		}
	}

	transform_.translate = Map::GetGrobalPos(static_cast<size_t>(onStagePos_.x), static_cast<size_t>(onStagePos_.y), static_cast<size_t>(onStagePos_.z)) + Vector3::kYIdentity;
}

bool OnBlockMoveComp::CheckLanding() const
{
	return pMap_->GetBoxType(onStagePos_) == Map::BoxType::kBox;
}

bool OnBlockMoveComp::CheckNormalMove() const
{
	// もし移動量が1以外だったらfalse
	if (moveDir_.Length() != 1.f) { return false; }

	const Vector3 afterPos = onStagePos_ + moveDir_;

	// もしマップ外に行っていた場合false
	if (afterPos.x < 0.f or afterPos.y < 0.f or afterPos.z < 0.f or afterPos.x >= Map::kMapX or afterPos.y >= Map::kMapY or afterPos.z >= Map::kMapZ) {
		return false;
	}

	// 移動先が足場ならtrue
	return pMap_->GetBoxType(onStagePos_ + moveDir_) == Map::BoxType::kBox;
}

bool OnBlockMoveComp::CheckJumpSideMove() const
{
	return false;
}

