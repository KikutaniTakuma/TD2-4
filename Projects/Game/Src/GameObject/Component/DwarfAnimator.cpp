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

	pSpriteComp_->SetTexture("./Resources/Enemy/slimeWalk.png");
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Enemy/slimeWallWalk.png");
	textureManager->LoadTexture("./Resources/Enemy/badSlimAttack.png");
	textureManager->LoadTexture("./Resources/Enemy/badSlimeWait.png");
}

void DwarfAnimatorComp::Update()
{
	bool isClimbing = pDwarfComp_->IsClimbing();

	// 0.0 ~ 1.0 の間で、1.0はギリ死なない
	pSpriteComp_->SetTexture(isClimbing ?  "./Resources/Enemy/slimeWallWalk.png" : "./Resources/Enemy/slimeWalk.png");

	spriteAnimator_->FlipHorizontal(pDwarfComp_->GetFacing() < 0);
}
