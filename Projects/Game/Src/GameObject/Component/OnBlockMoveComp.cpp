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
		else if (CheckJumpSideMove()) {

			// 移動
			onStagePos_ += moveDir_ * kJumpLength;
			// 移動量を0に
			moveDir_ = {};

		}
	}

	transform_.translate = Map::GetGrobalPos(static_cast<size_t>(onStagePos_.x), static_cast<size_t>(onStagePos_.y)) + Vector3::kYIdentity;
}

bool OnBlockMoveComp::CheckLanding() const
{
	return pMap_->GetBoxType(onStagePos_) == Map::BoxType::kGroundBlock;
}

bool OnBlockMoveComp::CheckNormalMove() const
{
	// もし移動量が1以外だったらfalse
	if (moveDir_.Length() != 1.f) { return false; }

	// 移動先が足場ならtrue
	return pMap_->GetBoxType(onStagePos_ + moveDir_) == Map::BoxType::kGroundBlock;
}

bool OnBlockMoveComp::CheckJumpSideMove() const
{

	// もし移動量が1以外だったらfalse
	if (moveDir_.Length() != 1.f) { return false; }

	// 移動先が足場ならtrue
	return pMap_->GetBoxType(onStagePos_ + moveDir_ * kJumpLength) == Map::BoxType::kGroundBlock;
}

