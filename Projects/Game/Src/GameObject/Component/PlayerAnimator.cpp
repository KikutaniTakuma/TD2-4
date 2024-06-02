#include "PlayerAnimator.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void PlayerAnimatorComp::StaticLoad()
{
	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Player/witchShot.png");
	textureManager->LoadTexture("./Resources/Player/witchWait.png");
	textureManager->LoadTexture("./Resources/Player/witchWalk.png");
	textureManager->LoadTexture("./Resources/Player/witchPickUp.png");
	textureManager->LoadTexture("./Resources/Player/witchHaveWalk.png");
	textureManager->LoadTexture("./Resources/Player/witchJumpDown.png");
	textureManager->LoadTexture("./Resources/Player/witchJumpUp.png");
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

	SetWait();

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
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	Lamb::SafePtr pad = Input::GetInstance()->GetGamepad();
	
	switch (currentState_)
	{
	case PlayerAnimatorComp::State::kAttack:
		AttackAnimationUpdate();
		break;
	case PlayerAnimatorComp::State::kHave:
		HaveAnimationUpdate();
		break;
	case PlayerAnimatorComp::State::kHaveUnder:
		HaveUnderAnimationUpdate();
		break;
	}

	SetState();

	// プレイヤーの方向に応じて反転
	spriteAnimator_->FlipHorizontal(pPlayerComp_->GetFacing() < 0);

	// 持った瞬間
	if (isHaveBlock_.OnEnter()) {
		haveParticle_->ParticleStart(pPlayerPickerComp_->GetBlockAffine().GetTranslate());
	}
	// 置いた瞬間
	else if (isHaveBlock_.OnExit()) {
		haveParticle_->ParticleStop();
	}

	// 持っている間のパーティクルのエミッターの場所を設定
	if (isHaveBlock_) {
		haveParticle_->emitterPos = pPlayerPickerComp_->GetBlockAffine().GetTranslate();
	}

	isShoting_ = isAttack_;
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

void PlayerAnimatorComp::SetState() {
	bool isAttack = pPlayerComp_->GetIsAttack();
	isHaveBlock_ = pPlayerPickerComp_->IsPicking();
	isUnderHave_ = pPlayerComp_->InputDown();
	isMove_ = pPlayerComp_->GetInputVec().x != 0.0f;
	isJumpAnimation_ = pPlayerComp_->GetIsJumping();
	isFallAnimation_ = pPlayerComp_->GetIsFalling();

	if (not isFallAnimation_) {
		isFallAnimation_ = false;
	}

	preState_ = currentState_;

	// 何よりも攻撃モーションは優先
	if (isAttack or isAttack_) {
		currentState_ = State::kAttack;
		if (isAttack or not isAttack_) {
			// 新しいアニメーションをするために再スタート
			spriteAnimator_->Stop();
			spriteAnimator_->Start();
			isAttack_ = true;
		}
	}
	// 次にジャンプアニメーション
	else if (isJumpAnimation_) {
		currentState_ = State::kJump;
	}
	// 次に落ちるアニメーション
	else if (isFallAnimation_) {
		currentState_ = State::kFall;
	}
	// ブロックを持ってる
	else if(isHaveBlock_) {
		// ブロックを下から持った瞬間
		if (isUnderHave_ or isUnderAnimation_) {
			currentState_ = State::kHaveUnder;
		}
		//// 移動してたら
		else if (isMove_) {
			currentState_ = State::kHaveMove;
		}
		// 下から持ち上げたアニメーションが終了した瞬間
		else if (isUnderAnimationEnd_.OnEnter()) {
			currentState_ = State::kHave;
		}
		// それ以外
		else {
			currentState_ = State::kHave;
		}
	}
	// 移動してら
	else if(isMove_){
		currentState_ = State::kMove;
	}
	// それ以外
	else {
		currentState_ = State::kWiat;
	}

	// ステータスが変わってたら
	if (currentState_ != preState_) {
		// 新しいアニメーションをするために再スタート
		spriteAnimator_->Stop();
		spriteAnimator_->Start();

		// それぞれのステートに応じて情報をセット
		switch (currentState_)
		{
		case PlayerAnimatorComp::State::kWiat:
			SetWait();
			break;
		case PlayerAnimatorComp::State::kMove:
			SetMove();
			break;
		case PlayerAnimatorComp::State::kAttack:
			SetAttack();
			break;
		case PlayerAnimatorComp::State::kHave:
			SetHave();
			break;
		case PlayerAnimatorComp::State::kHaveUnder:
			SetHaveUnder();
			break;
		case PlayerAnimatorComp::State::kHaveMove:
			SetHaveMove();
			break;
		case PlayerAnimatorComp::State::kJump:
			SetJump();
			break;
		case PlayerAnimatorComp::State::kFall:
			SetFall();
			break;
		default:
			throw Lamb::Error::Code<PlayerAnimatorComp>("The condition is strange", ErrorPlace);
			break;
		}
	}
	// アニメーションが止まってたら再スタート
	else if (not spriteAnimator_->GetIsActive() and not isJumpAnimation_ and not isFallAnimation_) {
		spriteAnimator_->Start();
	}
}

void PlayerAnimatorComp::SetWait() {
	pSpriteComp_->SetTexture("./Resources/Player/witchWait.png");
	spriteAnimator_->SetDuration(0.5f);
}

void PlayerAnimatorComp::SetMove() {
	pSpriteComp_->SetTexture("./Resources/Player/witchWalk.png");
	spriteAnimator_->SetDuration(0.1f);
}

void PlayerAnimatorComp::SetAttack() {
	spriteAnimator_->SetDuration(0.1f);
	pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
}

void PlayerAnimatorComp::SetHave() {
	spriteAnimator_->SetDuration(0.2f);
	pSpriteComp_->SetTexture("./Resources/Player/witchShot.png");
}

void PlayerAnimatorComp::SetHaveUnder() {
	isUnderAnimation_ = true;
	spriteAnimator_->SetDuration(0.2f);
	pSpriteComp_->SetTexture("./Resources/Player/witchPickUp.png");
	isUnderAnimationEnd_ = false;
}

void PlayerAnimatorComp::SetHaveMove() {
	spriteAnimator_->SetDuration(0.1f);
	pSpriteComp_->SetTexture("./Resources/Player/witchHaveWalk.png");
}

void PlayerAnimatorComp::SetJump()
{
	spriteAnimator_->SetDuration(0.1f);
	pSpriteComp_->SetTexture("./Resources/Player/witchJumpUp.png");
}

void PlayerAnimatorComp::SetFall()
{
	spriteAnimator_->SetDuration(0.1f);
	pSpriteComp_->SetTexture("./Resources/Player/witchJumpDown.png");
}

void PlayerAnimatorComp::AttackAnimationUpdate() {
	if (not spriteAnimator_->GetIsActive()) {
		isAttack_ = false;
	}
}

void PlayerAnimatorComp::HaveAnimationUpdate()
{
	// アニメーションナンバーが3を超えたら止める
	if (2 < spriteAnimator_->GetCurrentAnimationNumber()) {
		spriteAnimator_->Stop();
		spriteAnimator_->Start();
	}
}

void PlayerAnimatorComp::HaveUnderAnimationUpdate() {
	if (not spriteAnimator_->GetIsActive()) {
		isUnderAnimationEnd_ = true;
		isUnderAnimation_ = false;
	}
}
