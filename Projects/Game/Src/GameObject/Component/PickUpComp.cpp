#include "PickUpComp.h"
#include "Game/GameManager/GameManager.h"
#include <Drawers/DrawerManager.h>
#include <SoLib/Math/Math.hpp>
#include "DwarfComp.h"


void PickUpComp::Init()
{
	pDwarfComp_ = object_.GetComponent<DwarfComp>();
	pLocalBodyComp_ = object_.GetComponent<LocalBodyComp>();
	model_ = DrawerManager::GetInstance()->GetModel(kModelName_);
}

void PickUpComp::Draw(const Camera &camera) const
{
	float yDiff = 0.f;
	for (const auto &block : pickupBlockList_) {

		Vector3 pos = Vector3::kYIdentity * (1 + yDiff + (block.size_.y / 2 - 0.5f)) + transform_.translate;
		Matrix worldMat = SoLib::Math::Affine((Vector3::kZIdentity + block.size_) * 0.5f, Vector3::kZero, pos);
		model_->Draw(worldMat, camera.GetViewOthographics(), 0xFFFFFFFF, BlendType::kNone);

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
	auto [pickUpBlockData, center] = GameManager::GetInstance()->PickUp(localPos, GetBlockWeight());

	// もし、ブロックがない場合はfalse
	if (pickUpBlockData.GetWeight() == 0) { return false; }

	// ブロックのリストに追加
	pickupBlockList_.push_back(pickUpBlockData);

	return true;
}

void PickUpComp::ThrowAllBlocks()
{
	const Vector2 kDwarfBaseThrowSpeed_ = { 0.5f, 2.f };
	const Vector2 kDwarfMultipleSpeed_ = { 3.f, 1.f };
	const Vector2 kDwarfGravityPower_ = { 0.f, -15.f };

	Lamb::SafePtr gameManager = GameManager::GetInstance();

	float yDiff = 0.f;
	for (const auto &block : pickupBlockList_) {

		Vector2 pos = Vector2::kYIdentity * (1 + yDiff + (block.size_.y / 2 - 0.5f)) + pLocalBodyComp_->localPos_;


		gameManager->AddFallingBlock(pos, block.size_, false, Vector2{ kDwarfBaseThrowSpeed_.x + pDwarfComp_->GetFacing() * yDiff * kDwarfMultipleSpeed_.x, kDwarfBaseThrowSpeed_.y + yDiff * kDwarfMultipleSpeed_.y }, kDwarfGravityPower_);

		// yの値を加算する
		yDiff += block.size_.y;
	}

	pickupBlockList_.clear();
}
