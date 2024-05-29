#include "GameUIManager.h"
#include "Game/GameManager/GameManager.h"
#include "Utils/EngineInfo/EngineInfo.h"

void GameUIManager::Init(GameManager *pGameManager)
{
	pGameManager_ = pGameManager;

	gameTimerRender_ = std::make_unique<GameTimerRender>();
	gameTimerRender_->Init(pGameManager_->GetGameTimer());

	itemGauge_ = std::make_unique<ItemGauge>();
	itemGauge_->Initialize();

	// 時間がない演出用のテクスチャ
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	
	dangerTex_ = std::make_unique<Tex2DState>();
	// テクスチャ読み込み
	dangerTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/danger.png");
	// Scaleを設定
	dangerTex_->transform.scale = Lamb::ClientSize();
	dangerTex_->transform.translate.y = 10.0f;
	dangerTex_->transform.translate.z = -69.0f;
	dangerTex_->transform.CalcMatrix();
	dangerTex_->uvTransform.CalcMatrix();
	// 危ない演出を行う時間、今は30秒
	dangerTime_ = 30.0f;

	dangerColorEase_ = std::make_unique<Easeing>();

	damageParticleRed_ = std::make_unique<Particle>();
	damageParticleRed_->LoadSettingDirectory("Player-Damaged-Red");
	damageParticlePurple_ = std::make_unique<Particle>();
	damageParticlePurple_->LoadSettingDirectory("Player-Damaged-Purple");
	damageParticleYellow_ = std::make_unique<Particle>();
	damageParticleYellow_->LoadSettingDirectory("Player-Damaged-Yellow");
	damageParticleBlue_ = std::make_unique<Particle>();
	damageParticleBlue_->LoadSettingDirectory("Player-Damaged-Blue");
	damageParticleGreen_ = std::make_unique<Particle>();
	damageParticleGreen_->LoadSettingDirectory("Player-Damaged-Green");
}

void GameUIManager::Update([[maybe_unused]] const float deltaTime)
{
	gameTimerRender_->Update(deltaTime);
	itemGauge_->Update(pGameManager_->GetItemCount(), pGameManager_->GetClearItemCount());
	itemGauge_->Debug();

#ifdef _DEBUG
		ImGui::Begin("gaugePosX");
		ImGui::Text("%f", itemGauge_->GetReductionGaugePos().x);
		ImGui::End();
#endif // _DEBUG
	if (itemGauge_->GetItemReduction().OnEnter()) {
		Vector3 pos = itemGauge_->GetReductionGaugePos();
		pos += Vector3::kZIdentity * -50.0f;
		damageParticleRed_->ParticleStart(pos, Vector2::kIdentity);
		damageParticlePurple_->ParticleStart(pos, Vector2::kIdentity);
		damageParticleYellow_->ParticleStart(pos, Vector2::kIdentity);
		damageParticleBlue_->ParticleStart(pos, Vector2::kIdentity);
		damageParticleGreen_->ParticleStart(pos, Vector2::kIdentity);
	}

	damageParticleRed_->Update();
	damageParticlePurple_->Update();
	damageParticleYellow_->Update();
	damageParticleBlue_->Update();
	damageParticleGreen_->Update();

	float currentTime = pGameManager_->GetGameTimer()->GetDeltaTimer().GetNowFlame();
	float goalTime = pGameManager_->GetGameTimer()->GetDeltaTimer().GetGoalFlame();

	if (goalTime - currentTime <= dangerTime_) {
		if (not dangerColorEase_->GetIsActive()) {
			dangerColorEase_->Start(
				true,
				1.0f,
				Easeing::InOutSine
			);
		}
		dangerTex_->color = dangerColorEase_->Get(UintToVector4(white_), UintToVector4(whiteZeroAlpha_)).GetColorRGBA();
		dangerColorEase_->Update();


	}
}

void GameUIManager::Draw([[maybe_unused]] const Camera &camera) const
{
	gameTimerRender_->Draw(camera);
	itemGauge_->Draw(camera);

	float currentTime = pGameManager_->GetGameTimer()->GetDeltaTimer().GetNowFlame();
	float goalTime = pGameManager_->GetGameTimer()->GetDeltaTimer().GetGoalFlame();

	if (goalTime - currentTime <= dangerTime_) {
		tex2D_->Draw(
			dangerTex_->transform.matWorld_,
			dangerTex_->uvTransform.matWorld_,
			camera.GetViewOthographics(),
			dangerTex_->textureID,
			dangerTex_->color,
			BlendType::kUnenableDepthNormal
		);
	}

	damageParticleRed_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticlePurple_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleYellow_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleBlue_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
	damageParticleGreen_->Draw(
		camera.rotate,
		camera.GetViewOthographics(),
		BlendType::kUnenableDepthNormal
	);
}
