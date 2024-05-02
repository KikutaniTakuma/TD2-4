#include "GameEffectManager.h"
#include "Game/GameManager/GameManager.h"

void GameEffectManager::Init()
{
	pGameManager_ = GameManager::GetInstance();
	pMap_ = pGameManager_->GetMap();

	particles_.resize(20);
	for (auto& i : particles_) {
		i = std::make_unique<Particle>();

		i->LoadSettingDirectory("Block-Break");
	}
}

void GameEffectManager::Update([[maybe_unused]] float deltaTime)
{
	for (auto particle = particles_.begin(); const auto & blockPos : blockBreakPos_) {
		if (particle == particles_.end()) {
			break;
		}
		(*particle)->emitterPos.x = blockPos.x;
		(*particle)->emitterPos.y = blockPos.y;
		(*particle)->ParticleStart();
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
}
