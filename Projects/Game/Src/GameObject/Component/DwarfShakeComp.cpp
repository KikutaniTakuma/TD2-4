#include "DwarfShakeComp.h"

void DwarfShakeComp::Init()
{
	pDwarfComp_ = object_.AddComponent<DwarfComp>();
	pPickUpComp_ = object_.AddComponent<PickUpComp>();			// 持っているブロックの数はここ
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
}

void DwarfShakeComp::Update()
{
	// pSpriteComp_->offsetTransform_ <- テクスチャをどれだけズラすか

	// 0.0~1.0でどれだけブロックを持っているか。 1.0はギリ死なない。
	[[maybe_unused]] float pickUpCapacity = static_cast<float>(pPickUpComp_->GetBlockWeight()) / PickUpComp::kMaxWeight_;
}
