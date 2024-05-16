#include "DwarfAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void DwarfAnimatorComp::Init()
{
	pDwarfComp_ = object_.AddComponent<DwarfComp>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
	pAnimComp_ = object_.AddComponent<SpriteAnimatorComp>();	// UpdateとUV行列の転送を自動で行う

	object_.GetComponent<LocalBodyComp>()->drawScale_ = 2.0f;

	spriteAnimator_ = pAnimComp_->GetAnimator();
	spriteAnimator_->SetDuration(0.25f);
	spriteAnimator_->SetLoopAnimation(true);
	spriteAnimator_->SetAnimationNumber(3u);
	spriteAnimator_->Start();

	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureID_[0] = textureManager->LoadTexture("./Resources/Enemy/slimeWallWalk.png");
	textureID_[1] = textureManager->LoadTexture("./Resources/Enemy/slimeWalk.png");
	textureID_[2] = textureManager->LoadTexture("./Resources/Enemy/badSlimAttack.png");
	textureID_[3] = textureManager->LoadTexture("./Resources/Enemy/badSlimeWait.png");

	pSpriteComp_->SetTexture("./Resources/Enemy/slimeWalk.png");
}

void DwarfAnimatorComp::Update()
{
	bool isDrakDwarf = pDwarfComp_->GetIsDarkDwarf();
	pDwarfComp_->AttackTimer().IsFinish();

	if (isDrakDwarf) {
		bool isAttack = pDwarfComp_->AttackTimer().IsFinish();
		if (isAttack) {
			pSpriteComp_->SetTexture(textureID_[2]);
			spriteAnimator_->SetLoopAnimation(false);
			spriteAnimator_->Start();
		}
		else if(not spriteAnimator_->GetIsActive()){
			pSpriteComp_->SetTexture(textureID_[3]);
			spriteAnimator_->SetLoopAnimation(true);
			spriteAnimator_->Start();
		}
	}
	else {
		bool isClimbing = pDwarfComp_->IsClimbing();
		uint32_t textureIndex = (isClimbing ? 0 : 1);

		// 0.0 ~ 1.0 の間で、1.0はギリ死なない
		pSpriteComp_->SetTexture(textureID_[textureIndex]);
	}
	spriteAnimator_->FlipHorizontal(pDwarfComp_->GetFacing() < 0);
}
