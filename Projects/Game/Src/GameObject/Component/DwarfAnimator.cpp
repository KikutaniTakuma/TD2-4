#include "DwarfAnimator.h"

void DwarfAnimator::Init()
{
	pDwarfComp_ = object_.AddComponent<DwarfComp>();
	pPickUpComp_ = object_.AddComponent<PickUpComp>();			// 持っているブロックの数はここ
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
	pAnimComp_ = object_.AddComponent<SpriteAnimatorComp>();	// UpdateとUV行列の転送を自動で行う

	pAnimComp_->GetAnimator()->SetDuration(0.25f);
	pAnimComp_->GetAnimator()->SetLoopAnimation(true);
	pAnimComp_->GetAnimator()->SetAnimationNumber(6u);
}

void DwarfAnimator::Update()
{
	// 0.0 ~ 1.0 の間で、1.0はギリ死なない
	//float blockCapacity = static_cast<float>(pPickUpComp_->GetBlockWeight()) / PickUpComp::kMaxWeight_;

	pSpriteComp_->uvMatrix_ = pAnimComp_->GetAnimator()->GetUvMat4x4();
}
