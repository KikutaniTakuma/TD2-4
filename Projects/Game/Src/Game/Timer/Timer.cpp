#include "Timer.h"

void GameTimer::Init()
{
}

void GameTimer::Update(const float deltaTime)
{
	deltaTime;
	timer_.Update(deltaTime);
}


void GameTimerRender::Init([[maybe_unused]] GameTimer *gameTimer)
{
}

void GameTimerRender::Update([[maybe_unused]] const float deltaTime)
{
}

void GameTimerRender::Draw([[maybe_unused]] const Camera &camera) const
{
}
