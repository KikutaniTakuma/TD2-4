#include "DwarfComp.h"
#include "ModelComp.h"
#include <Drawers/DrawerManager.h>

void DwarfComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalBodyComp_->size_ = Vector2::kIdentity * 0.5f;

	Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel(GameManager::kDwarfModelName));
}

void DwarfComp::Start()
{
}

void DwarfComp::Update()
{
	// データの転送
	pLocalBodyComp_->TransfarData();
	transform_.translate.z = -1.5f;

}
