#include "DwarfAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void DwarfAnimatorComp::StaticLoad()
{
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureID_[0] = textureManager->LoadTexture("./Resources/Enemy/slimeWallWalk.png");
	textureID_[1] = textureManager->LoadTexture("./Resources/Enemy/slimeWalk.png");
	textureID_[2] = textureManager->LoadTexture("./Resources/Enemy/badSlimAttack.png");
	textureID_[3] = textureManager->LoadTexture("./Resources/Enemy/badSlimeWait.png");

}

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

	pSpriteComp_->SetTexture(textureID_[1]);
}

void DwarfAnimatorComp::Update()
{
	isDrakDwarf_ = pDwarfComp_->GetIsDarkDwarf();

	if (isDrakDwarf_) {
		if (isDrakDwarf_.OnEnter()) {
			spriteAnimator_->Stop();
		}
		bool isAttack = 0.9f < pDwarfComp_->AttackTimer().GetProgress() and pDwarfComp_->AttackTimer().GetProgress() <= 1.0f;
		if ((not spriteAnimator_->GetIsActive() and isAttack)/* or isDrakDwarf_.OnEnter()*/) {
			pSpriteComp_->SetTexture(textureID_[2]);
			spriteAnimator_->SetLoopAnimation(false);
			spriteAnimator_->SetDuration(0.25f);
			spriteAnimator_->Start();
		}
		else if (not spriteAnimator_->GetIsActive()) {
			pSpriteComp_->SetTexture(textureID_[3]);
			spriteAnimator_->SetLoopAnimation(false);
			spriteAnimator_->SetDuration(0.35f);
			spriteAnimator_->Start();
		}
	}
	else {
		bool isClimbing = pDwarfComp_->IsClimbing();
		uint32_t textureIndex = (isClimbing ? 0 : 1);

		spriteAnimator_->SetDuration(0.25f);

		// 0.0 ~ 1.0 の間で、1.0はギリ死なない
		pSpriteComp_->SetTexture(textureID_[textureIndex]);
	}
	spriteAnimator_->FlipHorizontal(pDwarfComp_->GetFacing() < 0);
}
