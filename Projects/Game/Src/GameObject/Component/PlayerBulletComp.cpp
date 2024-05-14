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

		const Vector2 centor = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f;
		// 右側に移動していた場合
		if (pHitMapComp_->hitNormal_.x < 0) {

			POINTS rightSide{
				.x = static_cast<int16_t>(centor.x + pLocalBodyComp_->size_.x / 2),
				.y = static_cast<int16_t>(centor.y)
			};

			GameManager::GetInstance()->GetMap()->BreakBlock(rightSide);
		}
		else {

			POINTS leftSide{
				.x = static_cast<int16_t>(centor.x - pLocalBodyComp_->size_.x / 2),
				.y = static_cast<int16_t>(centor.y)
			};

			GameManager::GetInstance()->GetMap()->BreakBlock(leftSide);
		}

		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

}
