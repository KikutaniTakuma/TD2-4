#include "PlayerBulletComp.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>

void PlayerBulletComp::StaticLoad()
{
	AudioManager *const audioManager = AudioManager::GetInstance();
	TextureManager *const textureManager = TextureManager::GetInstance();

	audioManager->Load("./Resources/Sounds/SE/blockHit.mp3");

	textureManager->LoadTexture("./Resources/Player/star.png");
}

void PlayerBulletComp::Init()
{
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pLocalRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pSpriteComp_ = object_.AddComponent<SpriteComp>();

	shotParticle_ = std::make_unique<Particle>();
	shotParticle_->LoadSettingDirectory("Magic");

	shotParticle_->ParticleStart(transform_.translate);
	shotParticle_->SetParticleScale(0.5f);

	pSpriteComp_->SetTexture(TextureManager::GetInstance()->LoadTexture("./Resources/Player/star.png"));
}

void PlayerBulletComp::Update()
{
	AudioManager *const audioManager = AudioManager::GetInstance();
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
			Audio *audio = nullptr;
			auto &block = map->GetBlockMap()->at(hitPos.y)[hitPos.x];
			block.AddDamage(1);
			if (block.GetDamage() >= 3) {
				audio = audioManager->Load("./Resources/Sounds/SE/blockHit.mp3");
				audio->Start(0.2f, false);
				gManager->BreakChainBlocks(hitPos);
			}
			else {
				audio = audioManager->Load("./Resources/Sounds/SE/blockHit.mp3");
				audio->Start(0.2f, false);
				gManager->HitChainBlocks(hitPos);
			}
		}
		object_.SetActive(false);
	}

	pLocalBodyComp_->TransfarData();

	shotParticle_->emitterPos = transform_.translate;

	shotParticle_->Update();

	pSpriteComp_->offsetTransform_.rotate *= Quaternion::MakeRotateZAxis(GetDeltaTime() * 360_deg);
	//pSpriteComp_->CalcTexUv();

}

void PlayerBulletComp::Draw(const Camera &camera) const {
	shotParticle_->Draw(
		camera.rotate,
		camera.GetViewOthographics()
	);
}
