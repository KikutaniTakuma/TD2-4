#include "GameEffectManager.h"
#include "Game/GameManager/GameManager.h"
#include <Drawers/DrawerManager.h>

void GameEffectManager::Init()
{
	pGameManager_ = GameManager::GetInstance();
	pMap_ = pGameManager_->GetMap();

	blockParticles_.resize(20);
	for (auto &i : blockParticles_) {
		i = std::make_unique<Particle>();

		i->LoadSettingDirectory("Block-Break");
	}
	for (auto &i : dwarfParticle_) {
		i = std::make_unique<Particle>();

		i->LoadSettingDirectory("MergeSlime");
	}
	AudioManager::GetInstance()->Load("./Resources/Sounds/SE/blockBreak.mp3");

	pSpriteDrawer = DrawerManager::GetInstance()->GetTexture2D();

	whiteTex_ = TextureManager::GetInstance()->GetWhiteTex();

}

void GameEffectManager::Update([[maybe_unused]] float deltaTime)
{
	/*for (auto particle = particles_.begin(); const auto & blockPosAndSize : blockBreakPos_) {
		if (particle == particles_.end()) {
			break;
		}
		Vector3 emitterpos = ToGrobal(blockPosAndSize.first);
		emitterpos.z = -10.0f;
		(*particle)->ParticleStart(emitterpos, blockPosAndSize.second);
	}*/

	if (blockBreakPos_.first != Block::BlockType::kNone) {
		Audio *audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/blockBreak.mp3");
		audio->Start(0.2f, false);
		auto particle = blockParticles_.begin();
		for (int32_t yi = 0; yi < BlockMap::kMapY; yi++) {
			for (int32_t xi = 0; xi < BlockMap::kMapX; xi++) {
				if (particle != blockParticles_.end()) {
					if (blockBreakPos_.second[yi][xi]) {
						(*particle)->ParticleStart({ ToGrobal(Vector2{static_cast<float>(xi), static_cast<float>(yi)}), -10.f }, Vector2::kIdentity);
						(*particle)->SetParticleScale(0.5f);
						(*particle)->SetParticleAllColor(Block::kBlockColor_[static_cast<uint32_t>(blockBreakPos_.first)]);
						++particle;
					}
				}
				else {
					break;
				}
			}
		}
	}

	for (const Vector2 pos : margeDwarfPos_) {
		auto particle = dwarfParticle_.begin() + dwarfParticleIndex_;
		if (particle != dwarfParticle_.end()) {
			(*particle)->ParticleStart({ ToGrobal(pos), -10.f }, Vector2::kIdentity);
			(*particle)->SetParticleScale(0.15f);
			++particle;
			dwarfParticleIndex_ = std::clamp(dwarfParticleIndex_+1, 0, 19);
		}
		else {
			break;
		}
	}

	for (auto i = dwarfParticle_.begin(); i != dwarfParticle_.end();) {
		auto &item = *i;
		item->Update();
		if (item->GetIsParticleStart().OnExit()) {
			std::swap(item, dwarfParticle_.at(dwarfParticleIndex_));
			dwarfParticleIndex_ = std::clamp(dwarfParticleIndex_-1, 0, 19);
		}
		else {
			i++;
		}
	}

	for (auto &i : blockParticles_) {
		i->Update();
	}
}

void GameEffectManager::Draw([[maybe_unused]] const Camera &camera) const
{
	for (auto &i : blockParticles_) {
		i->Draw(
			camera.rotate,
			camera.GetViewOthographics(),
			BlendType::kNormal
		);
	}
	for (auto &i : dwarfParticle_) {
		i->Draw(
			camera.rotate,
			camera.GetViewOthographics(),
			BlendType::kNormal
		);
	}
	for (uint32_t i = 0; i < fallingBlock_.size(); i++) {
		if (fallingBlock_[i]) {
			Vector2 centerPos{};
			centerPos.x = static_cast<float>(i);
			centerPos.y = std::lerp(0.f, static_cast<float>(BlockMap::kMapY), 0.5f);

			// グローバル座標に変換
			centerPos = ToGrobal(centerPos);

			Mat4x4 affineMat = Mat4x4::MakeScalar(Vector3{ 1.f,static_cast<float>(BlockMap::kMapY + 1),1.f });
			reinterpret_cast<Vector2 &>(affineMat.at(3)) = centerPos;	// x,y座標
			affineMat.at(3).at(2) = -1.f;						// z座標

			pSpriteDrawer->Draw(affineMat, Mat4x4::kIdentity, camera.GetViewOthographics(), whiteTex_, 0x55005555, BlendType::kNormal);
		}
	}

}

void GameEffectManager::Clear()
{
	blockBreakPos_ = {};
	dwarfDeadPos_.clear();
	margeDwarfPos_.clear();
}
