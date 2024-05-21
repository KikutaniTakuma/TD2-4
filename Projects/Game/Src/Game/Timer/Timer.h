#pragma once
#include "SoLib/SoLib/SoLib_Timer.h"
#include <Camera/Camera.h>

class GameTimer
{
public:
	GameTimer() = default;
	~GameTimer() = default;

	void Init();

	void TimerStart(const float goalTime) { timer_.Start(goalTime); }

	void Update(const float deltaTime);

	/// @brief Timerクラスの参照の取得
	/// @return タイマークラス
	const SoLib::Time::DeltaTimer &GetDeltaTimer() const { return timer_; }

	/// @brief タイムアップしたか
	/// @return 終わっていたらtrue
	inline bool IsFinish() const { return timer_.IsFinish(); }

private:
	SoLib::Time::DeltaTimer timer_;
};

class GameTimerRender {
public:

	GameTimerRender() = default;
	~GameTimerRender() = default;

	void Init(GameTimer *gameTimer);

	void Update(const float deltaTime);

	void Draw(const Camera &camera) const;

	/// @brief タイマーの保存
	/// @param gameTimer ゲームタイマー
	inline void SetTimer(GameTimer *gameTimer) { pGameTimer_ = gameTimer; }


private:

	GameTimer *pGameTimer_;

};