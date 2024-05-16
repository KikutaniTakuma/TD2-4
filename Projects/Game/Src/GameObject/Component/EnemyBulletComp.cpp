#include "EnemyBulletComp.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>

void EnemyBulletComp::Init()
{
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();
	pSpriteComp_->SetTexture(TextureManager::GetInstance()->GetWhiteTex());
}

void EnemyBulletComp::Update()
{
	// ステージ外なら死ぬ
	if (BlockMap::IsOutSide(pLocalBodyComp_->localPos_)) {

		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

}
