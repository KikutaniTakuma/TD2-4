#include "DwarfAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void DwarfAnimatorComp::Init()
{
	pDwarfComp_ = object_.AddComponent<DwarfComp>();
	pPickUpComp_ = object_.AddComponent<PickUpComp>();			// 持っているブロックの数はここ
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
	pAnimComp_ = object_.AddComponent<SpriteAnimatorComp>();	// UpdateとUV行列の転送を自動で行う

	object_.GetComponent<LocalBodyComp>()->drawScale_ = 2.0f;

	spriteAnimator_ = pAnimComp_->GetAnimator();
	spriteAnimator_->SetDuration(0.25f);
	spriteAnimator_->SetLoopAnimation(true);
	spriteAnimator_->SetAnimationNumber(5u);
	spriteAnimator_->Start();

	pSpriteComp_->SetTexture("./Resources/Enemy/dwarfWalk.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/Enemy/dwarfHaveGauge.png");
}

void DwarfAnimatorComp::Update()
{
	bool isHave = 0 < pPickUpComp_->GetBlockWeight();

	// 0.0 ~ 1.0 の間で、1.0はギリ死なない
	pSpriteComp_->SetTexture(isHave ? "./Resources/Enemy/dwarfHaveGauge.png" : "./Resources/Enemy/dwarfWalk.png");
	spriteAnimator_->SetVerticalSize(isHave ? 1.0f / 7.0f : 1.0f);
	if (isHave) {
		spriteAnimator_->SetVerticalPos(static_cast<float>(pPickUpComp_->GetBlockWeight()) / 7.0f);
	}
	else {
		spriteAnimator_->SetVerticalPos(0.0f);
	}


	spriteAnimator_->FlipHorizontal(pDwarfComp_->GetFacing() < 0);
}
