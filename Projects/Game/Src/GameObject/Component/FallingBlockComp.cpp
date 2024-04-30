#include "FallingBlockComp.h"
#include "ModelComp.h"
#include <Drawers/DrawerManager.h>

void FallingBlockComp::Init()
{
	pLocalPos_ = object_.AddComponent<LocalBodyComp>();

	Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel("Resources/Cube.obj"));
}

void FallingBlockComp::Start()
{

}

void FallingBlockComp::Update()
{
	// 時間差分
	const float deltaTime = GetDeltaTime();

	// 加速度を加算
	velocity_ += gravity_ * deltaTime;
	// 移動量を加算
	pLocalPos_->localPos_ += velocity_ * deltaTime;

	// グローバル空間に持ってくる
	pLocalPos_->TransfarData();
}
