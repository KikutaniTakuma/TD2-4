#include "PickUpComp.h"
#include "Game/GameManager/GameManager.h"

int32_t PickUpComp::GetBlockWeight() const
{
	int32_t result = 0;

	for (const auto &block : pickupBlockList_) {
		result += block.GetWeight();
	}

	return result;
}

bool PickUpComp::PickupTargetBlock(Vector2 localPos)
{
	// 持ち上げる
	PickUpBlockData pickUpBlockData = GameManager::GetInstance()->PickUp(localPos, GetBlockWeight());

	// もし、ブロックがない場合はfalse
	if (pickUpBlockData.GetWeight() == 0) { return false; }

	// ブロックのリストに追加
	pickupBlockList_.push_back(pickUpBlockData);

	return true;
}