#include "PlayerComp.h"
#include "../LambEngine/Input/Input.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SpriteComp.h"
#include "SoLib/Math/Math.hpp"

void PlayerComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2{ 0.8f,1.f };

	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();

	pPicker_ = object_.AddComponent<PlayerBlockPickerComp>();

	pHealthComp_ = object_.AddComponent<HealthComp>();
	pHealthComp_->SetMaxHealth(vMaxHealth_);
	pHealthComp_->Reset();

	Lamb::SafePtr spriteComp = object_.AddComponent<SpriteComp>();
	spriteComp->SetTexture("./Resources/uvChecker.png");
	spriteComp->CalcTexUv();

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
	preHealth_ = static_cast<int32_t>(pHealthComp_->GetNowHealth());


	pLocalRigidbody_->ApplyContinuousForce(kGrovity_);

	Input();
	transform_.translate = pLocalBodyComp_->GetGlobalPos();

	// 無敵時間の減少
	invincibleTime_ -= GetDeltaTime();
	invincibleTime_ = std::clamp(invincibleTime_, 0.f, *vMaxInvincibleTime_);
}

void PlayerComp::OnCollision([[maybe_unused]] GameObject *const other)
{

}

int32_t PlayerComp::InflictDamage(int32_t damage, const Vector2 acceleration)
{
	if (invincibleTime_ <= 0) {
		pHealthComp_->AddHealth(static_cast<float>(-damage));
		pLocalRigidbody_->ApplyInstantForce(acceleration);
		// 無敵時間を付与
		invincibleTime_ = vMaxInvincibleTime_;
	}
	return static_cast<int32_t>(pHealthComp_->GetNowHealth());
}

bool PlayerComp::GetIsDamage() const
{
	return static_cast<int32_t>(pHealthComp_->GetNowHealth()) != preHealth_;
}

void PlayerComp::FireBullet()
{
	GameManager::GetInstance()->AddPlayerBullet(pLocalBodyComp_->localPos_, Vector2::kXIdentity * (facing_ * *vBulletSpeed_));
}

void PlayerComp::Input()
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();

	//Vector2 velocity = pLocalRigidbody_->GetVelocity();

	inputVec_ = {};

	if (key->GetKey(DIK_A) or key->GetKey(DIK_LEFT)) {
		inputVec_ = -Vector2::kXIdentity * vMoveSpeed_;
	}
	if (key->GetKey(DIK_D) or key->GetKey(DIK_RIGHT)) {
		inputVec_ = +Vector2::kXIdentity * vMoveSpeed_;
	}

	if (inputVec_.x != 0) {
		pLocalRigidbody_->ApplyContinuousForce(inputVec_);

		facing_ = static_cast<int32_t>(SoLib::Math::Sign(inputVec_.x));
	}

	if (key->Pushed(DIK_X)) {
		if (not pPicker_->IsPicking()) {
			pPicker_->PickUp(facing_);
		}
		else {
			pPicker_->Drop(facing_);
		}
	}

	if (key->Pushed(DIK_SPACE)) {
		Audio *audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/shot.mp3");
		audio->Start(0.2f, false);
		isAttack_ = true;
		FireBullet();
	}
	else {
		isAttack_ = false;
	}

	// 着地している場合
	if (pHitMapComp_->hitNormal_.y > 0)
	{
		if (key->Pushed(DIK_Z)) {
			pLocalRigidbody_->ApplyInstantForce(Vector2::kYIdentity * 13.f);
		}
	}
	Vector2 velocity = pLocalRigidbody_->GetVelocity();
	velocity.y = 0;

	pLocalRigidbody_->ApplyContinuousForce(velocity * -0.05f * kGrovity_.Length());


}
