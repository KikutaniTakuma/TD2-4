#include "DwarfAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void DwarfAnimatorComp::StaticLoad()
{
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureID_[kNormal][0] = textureManager->LoadTexture("./Resources/Enemy/slimeWallWalk.png");
	textureID_[kNormal][1] = textureManager->LoadTexture("./Resources/Enemy/slimeWalk.png");
	textureID_[kNormal][2] = textureManager->LoadTexture("./Resources/Enemy/badSlimAttack.png");
	textureID_[kNormal][3] = textureManager->LoadTexture("./Resources/Enemy/badSlimeWait.png");

	textureID_[kDamage][0] = textureManager->LoadTexture("./Resources/Enemy/slimeWallWalkDamage.png");
	textureID_[kDamage][1] = textureManager->LoadTexture("./Resources/Enemy/slimeWalkDamage.png");
	textureID_[kDamage][2] = textureManager->LoadTexture("./Resources/Enemy/badSlimAttackDamage.png");
	textureID_[kDamage][3] = textureManager->LoadTexture("./Resources/Enemy/badSlimeWaitDamage.png");
	textureManager->LoadTexture("./Resources/Ball.png");
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

	pSpriteComp_->SetTexture(textureID_[kNormal][1]);

	absorptionParticle_ = std::make_unique<FlaskParticle>();
	absorptionParticle_->Resize(100);
	absorptionParticle_->SetDeathTime({ 0.5f, 1.0f });
	absorptionParticle_->SetFreq({ 0.01f, 0.05f });
	absorptionParticle_->SetEmitterPos(pDwarfComp_->transform_.translate);
	absorptionParticle_->SetRadius({ 0.5f, 1.0f });
	absorptionParticle_->SetRotate({ 0.0f, 6.28f });
	absorptionParticle_->SetColor(0xef00005f);
	absorptionParticle_->SetTextureID(DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png"));
	absorptionParticle_->SetParticleSize(Vector3::kIdentity * 0.2f, Vector3::kIdentity * 0.2f);
	absorptionParticle_->SetEndTranslate(pDwarfComp_->transform_.translate);
	absorptionParticle_->SetIsLoop(true);
}

void DwarfAnimatorComp::Update()
{
	isDrakDwarf_ = pDwarfComp_->GetIsDarkDwarf();
	Lamb::SafePtr map = GameManager::GetInstance()->GetMap();

	const auto &hitMap = map->GetHitMap();
	Lamb::SafePtr body = object_.GetComponent<LocalBodyComp>();
	Vector2 targetPos = body->localPos_ + Vector2::kIdentity * 0.5f;

	bool isHit = BlockMap::IsOutSide(targetPos) ? false : hitMap[static_cast<int32_t>(body->localPos_.y + 0.5f)][static_cast<int32_t>(body->localPos_.x + 0.5f)] and map->GetTimer().IsActive();

	if (isDrakDwarf_) {
		if (isDrakDwarf_.OnEnter()) {
			spriteAnimator_->Stop();
		}
		bool isAttack = 0.9f < pDwarfComp_->AttackTimer().GetProgress() and pDwarfComp_->AttackTimer().GetProgress() <= 1.0f;
		if ((not spriteAnimator_->GetIsActive() and isAttack)/* or isDrakDwarf_.OnEnter()*/) {
			spriteAnimator_->SetLoopAnimation(false);
			spriteAnimator_->SetDuration(0.25f);
			spriteAnimator_->Start();
			absorptionParticle_->Start();
		}
		else if (not spriteAnimator_->GetIsActive()) {
			spriteAnimator_->SetLoopAnimation(false);
			spriteAnimator_->SetDuration(0.35f);
			spriteAnimator_->Start();
		}
		if (isAttack) {
			pSpriteComp_->SetTexture(textureID_[isHit][2]);
		}
		else {
			pSpriteComp_->SetTexture(textureID_[isHit][3]);
		}
		if (0.99f < pDwarfComp_->AttackTimer().GetProgress()) {
			absorptionParticle_->Stop();
		}

		absorptionParticle_->SetEmitterPos(pDwarfComp_->transform_.translate);
		absorptionParticle_->SetEndTranslate(pDwarfComp_->transform_.translate);
		absorptionParticle_->Update();
	}
	else {
		bool isClimbing = pDwarfComp_->IsClimbing();
		uint32_t textureIndex = (isClimbing ? 0 : 1);

		spriteAnimator_->SetDuration(0.25f);

		// 0.0 ~ 1.0 の間で、1.0はギリ死なない
		pSpriteComp_->SetTexture(textureID_[isHit][textureIndex]);
	}
	spriteAnimator_->FlipHorizontal(pDwarfComp_->GetFacing() < 0);
}

void DwarfAnimatorComp::Draw(const Camera &camera) const {
	if (isDrakDwarf_) {
		absorptionParticle_->Draw(camera.GetViewOthographics());
	}
}
