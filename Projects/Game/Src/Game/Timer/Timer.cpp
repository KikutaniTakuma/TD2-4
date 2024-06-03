#include "Timer.h"
#include <Drawers/DrawerManager.h>
#include<imgui.h>


void GameTimer::Init()
{
}

void GameTimer::Update(const float deltaTime)
{
	deltaTime;
	timer_.Update(deltaTime);
}


void GameTimerRender::Init([[maybe_unused]] GameTimer *gameTimer){
	pGameTimer_ = gameTimer;

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	timerState_ = std::make_unique<Tex2DState>();
	timerState_->color = 0xffffffff;
	timerState_->transform.scale = { 240.0f,272.0f };
	timerState_->transform.translate = { 450.0f, 210.0f };
	timerState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timer.png");

	clockHandsState_ = std::make_unique<Tex2DState>();
	clockHandsState_->color = 0xffffffff;
	clockHandsState_->transform.scale = { 240.0f,272.0f };
	clockHandsState_->transform.translate = { 450.0f, 190.0f };
	clockHandsState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timerNeedle.png");

	moveClockHandsState_ = std::make_unique<Tex2DState>();
	moveClockHandsState_->color = 0xffffffff;
	moveClockHandsState_->transform.scale = { 240.0f,272.0f };
	moveClockHandsState_->transform.rotate = Quaternion::kIdentity;
	moveClockHandsState_->transform.translate = { 450.0f, 190.0f };
	moveClockHandsState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timerNeedle.png");

	timeNumberState_ = std::make_unique<Tex2DState>();
	timeNumberState_->color = 0xffffffff;
	timeNumberState_->transform.scale = { 48.0f,60.0f };
	timeNumberState_->transform.translate = { 500.0f, 190.0f };
	timeNumberState_->uvTransform.scale = { 0.1f,1.0f };
	timeNumberState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timeLimitNumber.png");

	timeNumberTenState_ = std::make_unique<Tex2DState>();
	timeNumberTenState_->color = 0xffffffff;
	timeNumberTenState_->transform.scale = { 48.0f,60.0f };
	timeNumberTenState_->transform.translate = { 450.0f, 190.0f };
	timeNumberTenState_->uvTransform.scale = { 0.1f,1.0f };
	timeNumberTenState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timeLimitNumber.png");

	timeNumberHundredState_ = std::make_unique<Tex2DState>();
	timeNumberHundredState_->color = 0xffffffff;
	timeNumberHundredState_->transform.scale = { 48.0f,60.0f };
	timeNumberHundredState_->transform.translate = { 400.0f, 190.0f };
	timeNumberHundredState_->uvTransform.scale = { 0.1f,1.0f };
	timeNumberHundredState_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Timer/timeLimitNumber.png");

	isFirstLoad_ = true;
}

void GameTimerRender::Update([[maybe_unused]] const float deltaTime){
	const auto& deltaTimer = pGameTimer_->GetDeltaTimer();

	float angle = (-Angle::PI2 * (deltaTimer.GetNowFlame() / deltaTimer.GetGoalFlame()));

	moveClockHandsState_->transform.rotate = Quaternion::MakeRotateZAxis(angle);

	TimeNumberMove();

	timerState_->transform.CalcMatrix();
	clockHandsState_->transform.CalcMatrix();
	moveClockHandsState_->transform.CalcMatrix();

	timeNumberState_->transform.CalcMatrix();
	timeNumberState_->uvTransform.CalcMatrix();
	timeNumberTenState_->transform.CalcMatrix();
	timeNumberTenState_->uvTransform.CalcMatrix();

	timeNumberHundredState_->transform.CalcMatrix();
	timeNumberHundredState_->uvTransform.CalcMatrix();
}

void GameTimerRender::Draw([[maybe_unused]] const Camera &camera) const{
	tex2D_->Draw(timerState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, timerState_->textureID, timerState_->color, BlendType::kNormal);


	tex2D_->Draw(clockHandsState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
		, clockHandsState_->textureID, clockHandsState_->color, BlendType::kNormal);
	
	tex2D_->Draw(moveClockHandsState_->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
			, moveClockHandsState_->textureID, moveClockHandsState_->color, BlendType::kNormal);

	tex2D_->Draw(timeNumberState_->transform.matWorld_, timeNumberState_->uvTransform.matWorld_, camera.GetViewOthographics()
		, timeNumberState_->textureID, timeNumberState_->color, BlendType::kNormal);

	tex2D_->Draw(timeNumberTenState_->transform.matWorld_, timeNumberTenState_->uvTransform.matWorld_, camera.GetViewOthographics()
		, timeNumberTenState_->textureID, timeNumberTenState_->color, BlendType::kNormal);

	tex2D_->Draw(timeNumberHundredState_->transform.matWorld_, timeNumberHundredState_->uvTransform.matWorld_, camera.GetViewOthographics()
		, timeNumberHundredState_->textureID, timeNumberHundredState_->color, BlendType::kNormal);

}

void GameTimerRender::TimeNumberMove() {
	const auto& deltaTimer = pGameTimer_->GetDeltaTimer();
	float nowRemainingTime = deltaTimer.GetGoalFlame() - deltaTimer.GetNowFlame();
	//現在の残り時間
	if (isFirstLoad_){
		nowRemainingTime = deltaTimer.GetGoalFlame();
		isFirstLoad_ = false;
	}
	else {
		nowRemainingTime = deltaTimer.GetGoalFlame() - deltaTimer.GetNowFlame();
	}
	

	timeNumberNum_ = static_cast<int32_t>(nowRemainingTime) % 10;
	timeNumberTenNum_ = static_cast<int32_t>(nowRemainingTime / 10.0f) % 10;
	timeNumberHundredNum_ = static_cast<int32_t>(nowRemainingTime / 100.0f) % 10;

	timeNumberState_->uvTransform.translate.x = timeNumberNum_ * 0.1f;
	timeNumberTenState_->uvTransform.translate.x = timeNumberTenNum_ * 0.1f;
	timeNumberHundredState_->uvTransform.translate.x = timeNumberHundredNum_ * 0.1f;
}
