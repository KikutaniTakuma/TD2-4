#include "DwarfShakeComp.h"
#include<LambEngine/Utils/Random/Random.h>

void DwarfShakeComp::Init()
{
	pDwarfComp_ = object_.AddComponent<DwarfComp>();
	pPickUpComp_ = object_.AddComponent<PickUpComp>();			// 持っているブロックの数はここ
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
}

void DwarfShakeComp::Update()
{
	if (pPickUpComp_->GetBlockWeight() == 0)
		return;
	// pSpriteComp_->offsetTransform_ <- テクスチャをどれだけズラすか

	// 0.0~1.0でどれだけブロックを持っているか。 1.0はギリ死なない。
	[[maybe_unused]] float pickUpCapacity = static_cast<float>(pPickUpComp_->GetBlockWeight()) / PickUpComp::kMaxWeight_;

	Vector2 shakeVec;

	shakeVec.x = Lamb::Random(-shakePower_.x, shakePower_.x);
	shakeVec.y = Lamb::Random(-shakePower_.y, shakePower_.y);

	pSpriteComp_->offsetTransform_.translate = shakeVec * pickUpCapacity;
	pSpriteComp_->offsetTransform_.CalcMatrix();
}
