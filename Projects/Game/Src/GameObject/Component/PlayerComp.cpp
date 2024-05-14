#include "PlayerComp.h"
#include "../LambEngine/Input/Input.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SpriteComp.h"
#include "SoLib/Math/Math.hpp"

void PlayerComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2::kIdentity;
	pLocalBodyComp_->drawScale_ = 1.f;

	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();

	pPicker_ = object_.AddComponent<PleyerBlockPickerComp>();

	Lamb::SafePtr spriteComp = object_.AddComponent<SpriteComp>();
	spriteComp->SetTexture("./Resources/uvChecker.png");
	spriteComp->CalcTexUv();
}

void PlayerComp::Update()
{
	//// 横移動速度を無くす
	//Vector2 velocity = pLocalRigidbody_->GetVelocity();
	//velocity.x = 0.f;
	//pLocalRigidbody_->SetVelocity(velocity);

	pLocalRigidbody_->ApplyContinuousForce(kGrovity_);

	Input();
	pLocalBodyComp_->TransfarData();
}

void PlayerComp::Input()
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();

	//Vector2 velocity = pLocalRigidbody_->GetVelocity();

	constexpr float moveSpeed = 10.f;

	Vector2 moveVec{};

	if (key->GetKey(DIK_A)) {
		moveVec = -Vector2::kXIdentity * moveSpeed;
	}
	if (key->GetKey(DIK_D)) {
		moveVec = +Vector2::kXIdentity * moveSpeed;
	}

	if (moveVec.x != 0) {
		pLocalRigidbody_->ApplyContinuousForce(moveVec);

		facing_ = static_cast<int32_t>(SoLib::Math::Sign(moveVec.x));
	}

	if (key->Pushed(DIK_Z)) {
		if (not pPicker_->IsPicking()) {
			pPicker_->PickUp(facing_);
		}
		else {
			pPicker_->Drop(facing_);
		}
	}


	// 着地している場合
	if (pHitMapComp_->hitNormal_.y > 0)
	{
		if (key->Pushed(DIK_SPACE)) {
			pLocalRigidbody_->ApplyInstantForce(Vector2::kYIdentity * 14.f);
		}
	}
	Vector2 velocity = pLocalRigidbody_->GetVelocity();
	velocity.y = 0;

	pLocalRigidbody_->ApplyContinuousForce(velocity * -0.05f * kGrovity_.Length());


}
