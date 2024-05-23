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
}

void GameUIManager::Update([[maybe_unused]] const float deltaTime)
{
	gameTimerRender_->Update(deltaTime);
	itemGauge_->Update(pGameManager_->GetItemCount(), pGameManager_->GetClearItemCount());
	itemGauge_->Debug();

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
			BlendType::kNormal
		);
	}
}
