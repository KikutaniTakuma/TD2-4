#include "PlayerBulletComp.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>

void PlayerBulletComp::Init()
{
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();
	pSpriteComp_->SetTexture(TextureManager::GetInstance()->GetWhiteTex());
}

void PlayerBulletComp::Update()
{
	// ぶつかっていたら返す
	if (pHitMapComp_->hitNormal_ != Vector2::kZero) {

		auto *gManager = GameManager::GetInstance();
		auto *map = gManager->GetMap();
		POINTS hitPos;

		const Vector2 centor = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f;
		// 右側に移動していた場合
		if (pHitMapComp_->hitNormal_.x < 0) {

			hitPos = {
				.x = static_cast<int16_t>(centor.x + pLocalBodyComp_->size_.x / 2),
				.y = static_cast<int16_t>(centor.y)
			};
		}
		else {

			hitPos = {
				.x = static_cast<int16_t>(centor.x - pLocalBodyComp_->size_.x / 2 - 0.05f),
				.y = static_cast<int16_t>(centor.y)
			};
		}

		// ステージ内なら
		if (not BlockMap::IsOutSide(hitPos)) {

			auto &block = map->GetBlockMap()->at(hitPos.y)[hitPos.x];
			block.AddDamage(1);
			if (block.GetDamage() >= 3) {
				gManager->BreakChainBlocks(hitPos);
			}
			else {
				gManager->HitChainBlocks(hitPos);
			}
		}
		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

}
