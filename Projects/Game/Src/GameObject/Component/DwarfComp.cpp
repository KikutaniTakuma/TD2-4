#include "DwarfComp.h"
#include "ModelComp.h"
#include <Drawers/DrawerManager.h>

void DwarfComp::Init()
{
	pPickUpComp_ = object_.AddComponent<PickUpComp>();

	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2::kIdentity * 0.5f;

	Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	Lamb::SafePtr bodyModel = modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel(GameManager::kDwarfModelName));
	bodyModel->color_ = 0xFF0000FF;
}

void DwarfComp::Start()
{
}

void DwarfComp::Update()
{

	// 持ち運び
	{
		// 現在の座標
		Vector2 localPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * 0.5f;
		// 足元のブロックの座標
		Vector2 downPos = localPos - Vector2::kYIdentity;

		// 足元にブロックがあるなら
		if (LocalBodyComp::pMap_->GetBoxType(downPos) == Map::BoxType::kGroundBlock)
		{
			// 持ち上げる
			pPickUpComp_->PickupTargetBlock(downPos);
		}


	}

	// もし埋まってたら上に登る
	ClimbUp();
	// 登ってない場合
	if (isClimbing_ == false)
	{
		// 落ちる処理を行う
		FallDown();

		// 一番高いブロックを探す
		// targetPos_ = FindNearHighestBox();

		// 移動先が定まっている場合
		if (targetPos_ != -Vector2::kIdentity)
		{
			// 移動を行う
			if (MoveBlockPos(targetPos_))   // ゴールに到達した場合
			{
				// ターゲットを無効にする
				targetPos_ = -Vector2::kIdentity;

			}
		}

		// 登っているブロックごとの処理
		//ChangeMovementTarget();
	}






	// データの転送
	pLocalBodyComp_->TransfarData();
	transform_.translate.z = -1.5f;

}

void DwarfComp::ClimbUp()
{
	const Vector2 localPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * 0.5f;
	// 現在のブロック
	auto blockType = LocalBodyComp::pMap_->GetBoxType(localPos);

	bool climbFlag = false;
	// ブロックの中にいる場合登る
	climbFlag |= blockType == Map::BoxType::kGroundBlock or blockType == Map::BoxType::kEnemyBlock;

	// 登るフラグが立っていたら登る
	if (climbFlag)
	{
		pLocalBodyComp_->localPos_.y += GetDeltaTime() * 2.f;
		isClimbing_ = true;
	}
	// 折れていたら登っていない
	else
	{
		isClimbing_ = false;
	}
}

void DwarfComp::FallDown()
{
	const float downPower = 0.1f;
	float afterPosY = pLocalBodyComp_->localPos_.y - downPower;
	// 落下先の座標
	const Vector2 downSide = Vector2{ pLocalBodyComp_->localPos_.x + 0.5f, afterPosY };
	if (afterPosY > 0)
	{
		// 落下先がブロックでない場合
		if (LocalBodyComp::pMap_->GetBoxType(downSide) == Map::BoxType::kNone)
		{
			// 下げる
			pLocalBodyComp_->localPos_.y -= downPower;
		}
		// 足場があった場合
		else
		{
			// 足場の上に強制的に置く
			pLocalBodyComp_->localPos_.y = std::floor(downSide.y) + 1.f;
		}
	}
	else
	{
		pLocalBodyComp_->localPos_.y = 0.f;
	}
}
