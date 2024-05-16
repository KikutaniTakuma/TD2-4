#include "PlayerAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void PlayerAnimatorComp::Init()
{
	pPlayerComp_ = object_.AddComponent<PlayerComp>();
	pPlayerRigidComp_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
	pAnimComp_ = object_.AddComponent<SpriteAnimatorComp>();	// UpdateとUV行列の転送を自動で行う

	object_.GetComponent<LocalBodyComp>()->drawScale_ = 2.0f;

	spriteAnimator_ = pAnimComp_->GetAnimator();
	spriteAnimator_->SetDuration(0.25f);
	spriteAnimator_->SetLoopAnimation(true);
	spriteAnimator_->SetAnimationNumber(4u);
	spriteAnimator_->Start();

	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Player/witchShot.png");
	textureManager->LoadTexture("./Resources/Player/witchWait.png");
	textureManager->LoadTexture("./Resources/Player/witchWalk.png");
	pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");
}

void PlayerAnimatorComp::Update()
{
	if (not spriteAnimator_->GetIsActive()) {
		spriteAnimator_->SetDuration(0.25f);
		spriteAnimator_->SetLoopAnimation(true);
		spriteAnimator_->SetAnimationNumber(4u);
		spriteAnimator_->Start();
		isAttackAnimation_ = false;
	}

	bool isAttack = pPlayerComp_->GetIsAttack();

	if (isAttack) {
		spriteAnimator_->SetDuration(0.1f);
		spriteAnimator_->SetLoopAnimation(false);
		spriteAnimator_->SetAnimationNumber(4u);
		spriteAnimator_->Start();
		pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
		isAttackAnimation_ = true;
	}
	else if(not isAttack and not isAttackAnimation_){
		if (pPlayerComp_->GetInputVec().x == 0.0f) {
			pSpriteComp_->SetTexture("./Resources/Player/witchWait.png");
			spriteAnimator_->SetAnimationNumber(2u);
			spriteAnimator_->SetDuration(0.5f);
		}
		else {
			pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");
			spriteAnimator_->SetAnimationNumber(4u);
			spriteAnimator_->SetDuration(0.25f);
		}
	}

	spriteAnimator_->FlipHorizontal(pPlayerComp_->GetFacing() < 0);
}
