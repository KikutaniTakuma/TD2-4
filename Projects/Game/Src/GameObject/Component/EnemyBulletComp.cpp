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

	shotParticle_ = std::make_unique<Particle>();
	shotParticle_->LoadSettingDirectory("Enemy-Magic");

	shotParticle_->ParticleStart(transform_.translate);
	shotParticle_->SetParticleScale(0.5f);
}

void EnemyBulletComp::Update()
{
	// ステージ外なら死ぬ
	if (BlockMap::IsOutSide(pLocalBodyComp_->localPos_)) {

		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

	shotParticle_->emitterPos = transform_.translate;
	shotParticle_->emitterPos.z = -2.0f;

	shotParticle_->Update();
}

void EnemyBulletComp::OnCollision(GameObject *const other)
{
	Lamb::SafePtr playerComp = other->GetComponent<PlayerComp>();
	if (playerComp) {

		playerComp->InflictDamage(1, Vector2{ 2.5f * SoLib::Math::Sign(pLocalRigidbody_->GetVelocity().x), 5.f });
	}

}

void EnemyBulletComp::Draw(const Camera& camera) const
{
	shotParticle_->Draw(
		camera.rotate,
		camera.GetViewOthographics()
	);
}
