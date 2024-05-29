#pragma once
#include <Camera/Camera.h>
#include <Game/Timer/Timer.h>
#include"Game/ItemGauge/ItemGauge.h"
#include "Utils/Easeing/Easeing.h"
#include <memory>
#include "Drawers/Particle/Particle.h"

class GameManager;

class GameUIManager {
public:
	GameUIManager() = default;
	~GameUIManager() = default;

	void Init(GameManager *pGameManager);

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

private:


	// タイマーの表示関数
	std::unique_ptr<GameTimerRender> gameTimerRender_;

	std::unique_ptr<ItemGauge> itemGauge_;

	GameManager *pGameManager_;


	// 今は30秒
	float dangerTime_;
	
	std::unique_ptr<Easeing> dangerColorEase_;
	const uint32_t white_ = 0xffffffff;
	const uint32_t whiteZeroAlpha_ = 0xffffff00;


	Texture2D* tex2D_;
	std::unique_ptr<Tex2DState> dangerTex_;

	std::unique_ptr<Particle> damageParticleRed_;
	std::unique_ptr<Particle> damageParticlePurple_;
	std::unique_ptr<Particle> damageParticleYellow_;
	std::unique_ptr<Particle> damageParticleBlue_;
	std::unique_ptr<Particle> damageParticleGreen_;
};