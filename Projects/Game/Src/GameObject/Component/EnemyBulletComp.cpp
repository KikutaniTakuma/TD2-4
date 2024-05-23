#include "EnemyBulletComp.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>
#include "PlayerComp.h"
#include "SoLib/Math/Math.hpp"

void EnemyBulletComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();
	pSpriteComp_->SetTexture(TextureManager::GetInstance()->LoadTexture("./Resources/Enemy/slimBullet.png"));
	pSpriteComp_->offsetTransform_.scale = Vector3::kIdentity * 2.f;
}

void EnemyBulletComp::Update()
{
	// ステージ外なら死ぬ
	if (BlockMap::IsOutSide(pLocalBodyComp_->localPos_)) {

		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

}

void EnemyBulletComp::OnCollision(GameObject *const other)
{
	Lamb::SafePtr playerComp = other->GetComponent<PlayerComp>();
	if (playerComp) {

		playerComp->InflictDamage(1, Vector2{ 2.5f * SoLib::Math::Sign(pLocalRigidbody_->GetVelocity().x), 5.f });
	}

}
