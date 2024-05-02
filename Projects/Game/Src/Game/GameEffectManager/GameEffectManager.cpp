#include "GameEffectManager.h"
#include "Game/GameManager/GameManager.h"
#include <Drawers/DrawerManager.h>

void GameEffectManager::Init()
{
	pGameManager_ = GameManager::GetInstance();
	pMap_ = pGameManager_->GetMap();

	particles_.resize(20);
	for (auto& i : particles_) {
		i = std::make_unique<Particle>();

		i->LoadSettingDirectory("Block-Break");
	}

	pSpriteDrawer = DrawerManager::GetInstance()->GetTexture2D();

	whiteTex_ = TextureManager::GetInstance()->GetWhiteTex();

}

void GameEffectManager::Update([[maybe_unused]] float deltaTime)
{
	for (auto particle = particles_.begin(); const auto & blockPos : blockBreakPos_) {
		if (particle == particles_.end()) {
			break;
		}
		Vector3 emitterpos = ToGrobal(blockPos);
		emitterpos.z = -10.0f;
		(*particle)->ParticleStart(emitterpos);
	}

	for (auto& i : particles_) {
		if (i->GetIsParticleStart()) {
			i->Update();
		}
	}
}

void GameEffectManager::Draw([[maybe_unused]] const Camera &camera) const
{
	for (auto& i : particles_) {
		if (i->GetIsParticleStart()) {
			i->Draw(
				camera.rotate,
				camera.GetViewOthographics(),
				BlendType::kNormal
			);
		}
	}
	if (fallingBlock_.first != -1) {
		Vector2 centerPos{};
		centerPos.x = std::lerp(static_cast<float>(fallingBlock_.first), static_cast<float>(fallingBlock_.second), 0.5f);
		centerPos.y = std::lerp(0.f, static_cast<float>(Map::kMapY), 0.5f);

		// グローバル座標に変換
		centerPos = ToGrobal(centerPos);

		Mat4x4 affineMat = Mat4x4::MakeScalar(Vector3{ std::abs(fallingBlock_.first - fallingBlock_.second) + 1.f,32.f,1.f });
		reinterpret_cast<Vector2 &>(affineMat.at(3)) = centerPos;
		affineMat.at(3).at(2) = -1.f;

		pSpriteDrawer->Draw(affineMat, Mat4x4::kIdentity, camera.GetViewOthographics(), whiteTex_, 0x55005555, BlendType::kNormal);
	}
}

void GameEffectManager::Clear()
{
	blockBreakPos_.clear();
	dwarfDeadPos_.clear();
	fallingBlock_ = { -1,-1 };
}
