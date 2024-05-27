#include "PlayerAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void PlayerAnimatorComp::StaticLoad()
{
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Player/witchShot.png");
	textureManager->LoadTexture("./Resources/Player/witchWait.png");
	textureManager->LoadTexture("./Resources/Player/witchWalk.png");
}

void PlayerAnimatorComp::Init()
{
	pPlayerComp_ = object_.AddComponent<PlayerComp>();
	pPlayerRigidComp_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();			// 描画を自動で行う
	pAnimComp_ = object_.AddComponent<SpriteAnimatorComp>();	// UpdateとUV行列の転送を自動で行う
	pPlayerPickerComp_ = object_.AddComponent<PlayerBlockPickerComp>();
	pMapHitComp_ = object_.AddComponent<LocalMapHitComp>();

	object_.GetComponent<LocalBodyComp>()->drawScale_ = 2.0f;

	spriteAnimator_ = pAnimComp_->GetAnimator();
	spriteAnimator_->SetDuration(0.25f);
	spriteAnimator_->SetLoopAnimation(true);
	spriteAnimator_->SetAnimationNumber(4u);
	spriteAnimator_->Start();

	pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");

	haveParticle_ = std::make_unique<Particle>();
	haveParticle_->LoadSettingDirectory("MagicHand");
	shotParticle_ = std::make_unique<Particle>();
	shotParticle_->LoadSettingDirectory("Magic");
	smokeParticle_ = std::make_unique<Particle>();
	smokeParticle_->LoadSettingDirectory("Smoke");

	damageParticleRed_ = std::make_unique<Particle>();
	damageParticleRed_->LoadSettingDirectory("Player-Damaged-Red");
	damageParticlePurple_ = std::make_unique<Particle>();
	damageParticlePurple_->LoadSettingDirectory("Player-Damaged-Purple");
	damageParticleYellow_ = std::make_unique<Particle>();
	damageParticleYellow_->LoadSettingDirectory("Player-Damaged-Yellow");
	damageParticleBlue_ = std::make_unique<Particle>();
	damageParticleBlue_->LoadSettingDirectory("Player-Damaged-Blue");
	damageParticleGreen_ = std::make_unique<Particle>();
	damageParticleGreen_->LoadSettingDirectory("Player-Damaged-Green");
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
		spriteAnimator_->Start();
		pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
		isAttackAnimation_ = true;
	}
	else if (not isAttack and not isAttackAnimation_) {
		if (pPlayerComp_->GetInputVec().x == 0.0f) {
			pSpriteComp_->SetTexture("./Resources/Player/witchWait.png");
			spriteAnimator_->SetDuration(0.5f);
		}
		else {
			pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");
			spriteAnimator_->SetDuration(0.25f);
		}
	}

	spriteAnimator_->FlipHorizontal(pPlayerComp_->GetFacing() < 0);

	isPicking_ = pPlayerPickerComp_->IsPicking();

	if (isPicking_.OnEnter()) {
		haveParticle_->ParticleStart(pPlayerPickerComp_->GetBlockAffine().GetTranslate());
	}
	else if (isPicking_.OnExit()) {
		haveParticle_->ParticleStop();
	}

	if (isPicking_) {
		haveParticle_->emitterPos = pPlayerPickerComp_->GetBlockAffine().GetTranslate();
	}

	isShoting_ = isAttackAnimation_;
	if (isShoting_.OnEnter()) {
		shotParticle_->ParticleStart(transform_.translate + Vector3{ pPlayerComp_->GetFacing() * 0.5f,0.f,-50.f }, Vector2::kIdentity);
		shotParticle_->SetParticleScale(0.5f);
	}
	else if (isShoting_.OnExit()) {
		shotParticle_->ParticleStop();
	}
	if (isShoting_) {
		shotParticle_->emitterPos = transform_.translate + Vector3{ pPlayerComp_->GetFacing() * 0.5f,0.f,-50.f };
	}

	isLanding_ = pMapHitComp_->hitNormal_.y > 0.f;
	if (isLanding_.OnEnter()) {
		smokeParticle_->ParticleStart(transform_.translate + Vector3{ 0.f,-0.5f,-50.f }, Vector2::kIdentity);
		smokeParticle_->SetParticleScale(0.25f);
	}
	if (isLanding_) {
		smokeParticle_->emitterPos = transform_.translate + Vector3{ 0.f,-0.5f,-50.f };
	}

	isDamage_ = pPlayerComp_->GetIsDamage();
	if (isDamage_.OnEnter()) {
		damageParticleRed_->ParticleStart(transform_.translate + Vector3{ 0.f, 0.f,-50.f }, Vector2::kIdentity);
		damageParticleRed_->SetParticleScale(0.5f);
		damageParticlePurple_->ParticleStart(transform_.translate + Vector3{ 0.f, 0.f,-50.f }, Vector2::kIdentity);
		damageParticlePurple_->SetParticleScale(0.5f);
		damageParticleYellow_->ParticleStart(transform_.translate + Vector3{ 0.f, 0.f,-50.f }, Vector2::kIdentity);
		damageParticleYellow_->SetParticleScale(0.5f);
		damageParticleBlue_->ParticleStart(transform_.translate + Vector3{ 0.f, 0.f,-50.f }, Vector2::kIdentity);
		damageParticleBlue_->SetParticleScale(0.5f);
		damageParticleGreen_->ParticleStart(transform_.translate + Vector3{ 0.f, 0.f,-50.f }, Vector2::kIdentity);
		damageParticleGreen_->SetParticleScale(0.5f);
	}
	if (isDamage_) {
		damageParticleRed_->emitterPos = transform_.translate + Vector3{ 0.f, 0.f,-50.f };
		damageParticlePurple_->emitterPos = transform_.translate + Vector3{ 0.f, 0.f,-50.f };
		damageParticleYellow_->emitterPos = transform_.translate + Vector3{ 0.f, 0.f,-50.f };
		damageParticleBlue_->emitterPos = transform_.translate + Vector3{ 0.f, 0.f,-50.f };
		damageParticleGreen_->emitterPos = transform_.translate + Vector3{ 0.f, 0.f,-50.f };

	}

	damageParticleRed_->Update();
	damageParticlePurple_->Update();
	damageParticleYellow_->Update();
	damageParticleBlue_->Update();
	damageParticleGreen_->Update();


	smokeParticle_->Update();

	shotParticle_->Update();

	haveParticle_->Update();

	if (float time = pPlayerComp_->GetInvincibleTime(); time) {
		pSpriteComp_->color_ = (0xFFFFFF00 | static_cast<uint32_t>(std::lerp(0x33, 0xFF, std::round(std::fmodf(time, 0.2f) / 0.2f))));
	}
	else {
		pSpriteComp_->color_ = 0xFFFFFFFF;
	}
}

void PlayerAnimatorComp::Draw(const Camera &camera) const {
	// 描画順を変えるコスト考えて無理矢理変えた
	pSpriteComp_->Draw(camera);
	damageParticleRed_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticlePurple_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleYellow_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleBlue_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleGreen_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);



	smokeParticle_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	shotParticle_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	haveParticle_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
}
