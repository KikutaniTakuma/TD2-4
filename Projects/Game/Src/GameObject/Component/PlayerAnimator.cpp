#include "PlayerAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void PlayerAnimatorComp::StaticLoad()
{
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Player/witchShot.png");
	textureManager->LoadTexture("./Resources/Player/witchWait.png");
	textureManager->LoadTexture("./Resources/Player/witchWalk.png");
	textureManager->LoadTexture("./Resources/Player/witchPickUp.png");
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
	spriteAnimator_->SetLoopAnimation(false);
	spriteAnimator_->SetAnimationNumber(4u);
	spriteAnimator_->Start();

	pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");

	haveParticle_ = std::make_unique<Particle>();
	haveParticle_->LoadSettingDirectory("MagicHand");
	shotParticle_ = std::make_unique<Particle>();
	shotParticle_->LoadSettingDirectory("Magic");
	smokeParticle_ = std::make_unique<Particle>();
	smokeParticle_->LoadSettingDirectory("Smoke");

	isUnderAnimationEnd_ = true;

	/*damageParticleRed_ = std::make_unique<Particle>();
	damageParticleRed_->LoadSettingDirectory("Player-Damaged-Red");
	damageParticlePurple_ = std::make_unique<Particle>();
	damageParticlePurple_->LoadSettingDirectory("Player-Damaged-Purple");
	damageParticleYellow_ = std::make_unique<Particle>();
	damageParticleYellow_->LoadSettingDirectory("Player-Damaged-Yellow");
	damageParticleBlue_ = std::make_unique<Particle>();
	damageParticleBlue_->LoadSettingDirectory("Player-Damaged-Blue");
	damageParticleGreen_ = std::make_unique<Particle>();
	damageParticleGreen_->LoadSettingDirectory("Player-Damaged-Green");*/
}

void PlayerAnimatorComp::Update()
{
	/*if (not spriteAnimator_->GetIsActive()) {
		spriteAnimator_->SetDuration(0.25f);
		spriteAnimator_->SetLoopAnimation(true);
		spriteAnimator_->SetAnimationNumber(4u);
		spriteAnimator_->Start();
		isAttackAnimation_ = false;
	}*/
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	Lamb::SafePtr pad = Input::GetInstance()->GetGamepad();
	bool isAttack = pPlayerComp_->GetIsAttack();
	isHaveBlock_ = pPlayerPickerComp_->IsPicking();
	bool isUnderHave = pPlayerPickerComp_->GetFacing() == 0 and isHaveBlock_;
	if (not isUnderHave and isUnderAnimation_) {
		isUnderAnimation_ = false;
	}
	if (not isHaveBlock_) {
		isNormalPickAnimation_ = false;
	}
	if (isAttackAnimation_ and not spriteAnimator_->GetIsActive() and not isAttack) {
		isAttackAnimation_ = false;
	}

	// もし攻撃してたら攻撃アニメーション
	if (isAttack) {
		spriteAnimator_->SetDuration(0.1f);
		spriteAnimator_->SetLoopAnimation(false);
		spriteAnimator_->Start();
		pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
		isAttackAnimation_ = true;
	}
	// もしブロックを持った瞬間
	else if (isHaveBlock_) {
		// もし下向きのブロックを持ったら
		if(isUnderHave and isHaveBlock_.OnEnter()){
			isUnderAnimation_ = true;
			spriteAnimator_->SetDuration(0.1f);
			pSpriteComp_->SetTexture("./Resources/Player/witchPickUp.png");
			spriteAnimator_->Start();
			isUnderAnimationEnd_ = false;
		}
		else if(isUnderHave and isUnderAnimation_ and not spriteAnimator_->GetIsActive()){
			isUnderAnimationEnd_ = true;
			isUnderAnimation_ = false;
			isNormalPickAnimation_ = true;
		}
		else if(isUnderAnimationEnd_){
			spriteAnimator_->SetDuration(0.2f);
			if (3 < spriteAnimator_->GetCurrentAnimationNumber()) {
				spriteAnimator_->Stop();
			}
			if (not spriteAnimator_->GetIsActive()) {
				spriteAnimator_->Start();
			}
			pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
			isNormalPickAnimation_ = true;
		}
	}
	// もし攻撃してなくて攻撃アニメーションをしてなかったら
	else if (not isAttack and not isAttackAnimation_) {
		// 移動してない
		if (pPlayerComp_->GetInputVec().x == 0.0f) {
			pSpriteComp_->SetTexture("./Resources/Player/witchWait.png");
			spriteAnimator_->SetDuration(0.5f);
		}
		// 移動してる
		else {
			pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");
			spriteAnimator_->SetDuration(0.25f);
		}
	}
	if (not spriteAnimator_->GetIsActive()) {
		spriteAnimator_->Start();
	}

	// プレイヤーの方向に応じて反転
	spriteAnimator_->FlipHorizontal(pPlayerComp_->GetFacing() < 0);

	// 持っているか
	isPicking_ = isHaveBlock_;

	// 持った瞬間
	if (isPicking_.OnEnter()) {
		haveParticle_->ParticleStart(pPlayerPickerComp_->GetBlockAffine().GetTranslate());
	}
	// 置いた瞬間
	else if (isPicking_.OnExit()) {
		haveParticle_->ParticleStop();
	}

	// 持っている間のパーティクルのエミッターの場所を設定
	if (isPicking_) {
		haveParticle_->emitterPos = pPlayerPickerComp_->GetBlockAffine().GetTranslate();
	}

	isShoting_ = isAttackAnimation_;
	// 攻撃した瞬間
	if (isShoting_.OnEnter()) {
		shotParticle_->ParticleStart(transform_.translate + Vector3{ pPlayerComp_->GetFacing() * 0.5f,0.f,-50.f }, Vector2::kIdentity);
		shotParticle_->SetParticleScale(0.5f);
	}
	// 攻撃が終わった瞬間
	else if (isShoting_.OnExit()) {
		shotParticle_->ParticleStop();
	}
	// 撃っている間のパーティクルのエミッターの場所を設定
	if (isShoting_) {
		shotParticle_->emitterPos = transform_.translate + Vector3{ pPlayerComp_->GetFacing() * 0.5f,0.f,-50.f };
	}

	// 着地のパーティクル
	isLanding_ = pMapHitComp_->hitNormal_.y > 0.f;
	if (isLanding_.OnEnter()) {
		smokeParticle_->ParticleStart(transform_.translate + Vector3{ 0.f,-0.5f,-50.f }, Vector2::kIdentity);
		smokeParticle_->SetParticleScale(0.25f);
	}
	if (isLanding_) {
		smokeParticle_->emitterPos = transform_.translate + Vector3{ 0.f,-0.5f,-50.f };
	}

	isDamage_ = pPlayerComp_->GetIsDamage();
	/*if (isDamage_.OnEnter()) {
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
	damageParticleGreen_->Update();*/


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
	/*damageParticleRed_->Draw(
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
	);*/



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
