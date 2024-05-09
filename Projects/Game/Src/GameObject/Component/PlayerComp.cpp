#include "PlayerComp.h"
#include "../LambEngine/Input/Input.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SpriteComp.h"
#include "LocalMapHitComp.h"

void PlayerComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2::kIdentity;
	pLocalBodyComp_->drawScale_ = 1.f;

	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	object_.AddComponent<LocalMapHitComp>();

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

	Input();
	pLocalBodyComp_->TransfarData();
}

void PlayerComp::Input()
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();

	Vector2 velocity = pLocalRigidbody_->GetVelocity();

	constexpr float moveSpeed = 10.f;

	if (key->GetKey(DIK_A)) {
		pLocalRigidbody_->ApplyContinuousForce(-Vector2::kXIdentity * moveSpeed);
	}
	if (key->GetKey(DIK_D)) {
		pLocalRigidbody_->ApplyContinuousForce(+Vector2::kXIdentity * moveSpeed);
	}


}
