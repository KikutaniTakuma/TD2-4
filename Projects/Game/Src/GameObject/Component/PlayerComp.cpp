#include "PlayerComp.h"
#include "../LambEngine/Input/Input.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SpriteComp.h"

void PlayerComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2::kIdentity;
	pLocalBodyComp_->drawScale_ = 1.f;

	Lamb::SafePtr spriteComp = object_.AddComponent<SpriteComp>();
	spriteComp->SetTexture("./Resources/uvChecker.png");
	spriteComp->CalcTexUv();
}

void PlayerComp::Update()
{
	Input();
	pLocalBodyComp_->TransfarData();
}

void PlayerComp::Input()
{
	Lamb::SafePtr key = Input::GetInstance()->GetKey();

	if (key->GetKey(DIK_A)) {
		pLocalBodyComp_->localPos_.x -= 1;
	}
	if (key->GetKey(DIK_D)) {
		pLocalBodyComp_->localPos_.x += 1;
	}


}
