#include "PickUpComp.h"
#include "Game/GameManager/GameManager.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>

void PickUpComp::Init()
{
	model_ = DrawerManager::GetInstance()->GetModel(kModelName_);
}

void PickUpComp::Draw(const Camera &camera) const
{
	float yDiff = 0.f;
	for (const auto &block : pickupBlockList_) {

		Vector3 pos = Vector3::kYIdentity * (1 + yDiff + (block.size_.y / 2 - 0.5f)) + transform_.translate;
		Matrix worldMat = SoLib::Math::Affine((Vector3::kZIdentity + block.size_) * 0.5f, Vector3::kZero, pos);
		model_->Draw(worldMat, camera.GetViewProjection(), 0xFFFFFFFF, BlendType::kNone);

		// yの値を加算する
		yDiff += block.size_.y;
	}
}

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