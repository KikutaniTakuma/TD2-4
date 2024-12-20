#include "PlayerComp.h"
#include "../LambEngine/Input/Input.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SpriteComp.h"
#include "SoLib/Math/Math.hpp"

void PlayerComp::StaticLoad()
{
	[[maybe_unused]] AudioManager *const audioManager = AudioManager::GetInstance();
	[[maybe_unused]] TextureManager *const textureManager = TextureManager::GetInstance();

	audioManager->Load("./Resources/Sounds/SE/shot.mp3");
}

void PlayerComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2{ 0.6f,1.f };

	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();

	pPicker_ = object_.AddComponent<PlayerBlockPickerComp>();

	Lamb::SafePtr spriteComp = object_.AddComponent<SpriteComp>();
	//spriteComp->SetTexture("./Resources/uvChecker.png");
	spriteComp->CalcTexUv();

	damege_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/damege.mp3");
	shot_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/shot.mp3");

	isAttack_ = false;
}

void PlayerComp::Update()
{
	// 横移動速度を無くす
	if (GetInputVec().x == 0.f and pHitMapComp_->hitNormal_.y > 0.f) {
		Vector2 velocity = pLocalRigidbody_->GetVelocity();
		velocity = { -velocity.x * vFriction_, 0 };
		pLocalRigidbody_->ApplyContinuousForce(velocity);
	}

	pLocalRigidbody_->ApplyContinuousForce(kGrovity_);

	isJumping_ = pLocalRigidbody_->GetVelocity().y > 0;
	isFalling_ = pLocalRigidbody_->GetVelocity().y < 0;
	transform_.translate = { pLocalBodyComp_->GetGlobalPos(), -40.f };

	// 無敵時間の減少
	invincibleTime_ -= GetDeltaTime();
	invincibleTime_ = std::clamp(invincibleTime_, 0.f, *vMaxInvincibleTime_);

	fireCoolTime_ -= GetDeltaTime();
	fireCoolTime_ = std::clamp(fireCoolTime_, 0.f, *vFireCoolTime_);
}

void PlayerComp::OnCollision([[maybe_unused]] GameObject *const other)
{

}

bool PlayerComp::InflictDamage(const Vector2 acceleration)
{
	if (invincibleTime_ <= 0) {
		damageFlag_ = true;
		pLocalRigidbody_->ApplyInstantForce(acceleration);
		// 無敵時間を付与
		invincibleTime_ = vMaxInvincibleTime_;
		GameManager::GetInstance()->RemovePoint(vDamageDropCount_);

		damege_->Start(0.2f, false);
	}
	return damageFlag_.OnEnter();
}

bool PlayerComp::GetIsDamage() const
{
	return damageFlag_.OnEnter();
}

void PlayerComp::FireBullet()
{
	GameManager::GetInstance()->AddPlayerBullet(pLocalBodyComp_->localPos_, Vector2::kXIdentity * (facing_ * *vBulletSpeed_));
	fireCoolTime_ = vFireCoolTime_;
}

void PlayerComp::Input()
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	Lamb::SafePtr pad = Input::GetInstance()->GetGamepad();

	//Vector2 velocity = pLocalRigidbody_->GetVelocity();

	inputVec_ = {};

	if (key->GetKey(DIK_A) or key->GetKey(DIK_LEFT)) {
		inputVec_ = -Vector2::kXIdentity * vMoveSpeed_;
	}
	if (key->GetKey(DIK_D) or key->GetKey(DIK_RIGHT)) {
		inputVec_ = +Vector2::kXIdentity * vMoveSpeed_;
	}
	if (inputVec_.x == 0.f) {
		inputVec_.x = pad->GetStick(Gamepad::Stick::LEFT_X);
		if (std::abs(inputVec_.x) < 0.2f) {
			inputVec_.x = 0.f;
		}
		inputVec_.x *= vMoveSpeed_;
	}

	if (inputVec_.x != 0) {
		pLocalRigidbody_->ApplyContinuousForce(inputVec_);

		facing_ = static_cast<int32_t>(SoLib::Math::Sign(inputVec_.x));
	}

	if (key->Pushed(DIK_X) or pad->Pushed(Gamepad::Button::Y) or pad->Pushed(Gamepad::Button::X)) {
		const bool isDown = InputDown();
		if (not pPicker_->IsPicking()) {
			pPicker_->PickUp(isDown ? 0 : facing_);
		}
		else {
			pPicker_->Drop(isDown ? 0 : facing_);
		}
	}

	if (fireCoolTime_ <= 0.f and (key->Pushed(DIK_Z) or pad->Pushed(Gamepad::Button::RIGHT_SHOULDER))) {
		shot_->Start(0.2f, false);
		isAttack_ = true;
		FireBullet();
	}
	else {
		isAttack_ = false;
	}

	// 着地している場合
	if (pHitMapComp_->hitNormal_.y > 0) {
		if (key->Pushed(DIK_SPACE) or pad->GetButton(Gamepad::Button::A) or pad->GetButton(Gamepad::Button::B)) {
			pLocalRigidbody_->ApplyInstantForce(Vector2::kYIdentity * 13.f);
			isJumping_ = true;
		}
	}
	Vector2 velocity = pLocalRigidbody_->GetVelocity();
	velocity.y = 0;

	pLocalRigidbody_->ApplyContinuousForce(velocity * -0.05f * kGrovity_.Length());


}

bool PlayerComp::InputDown() const
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	Lamb::SafePtr pad = Input::GetInstance()->GetGamepad();
	return  key->GetKey(DIK_S) or key->GetKey(DIK_DOWN) or pad->GetStick(Gamepad::Stick::LEFT_Y) < -0.2f;
}
