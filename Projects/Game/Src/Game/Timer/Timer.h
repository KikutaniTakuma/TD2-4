#pragma once
#include "SoLib/SoLib/SoLib_Timer.h"
#include <Camera/Camera.h>
#include "SoLib/Containers/VItem.h"
#include "Drawers/Texture2D/Texture2D.h"
#include"SoLib/Math/Angle.h"

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
	//時計の数字の処理
	void TimeNumberMove();

private:

	GameTimer *pGameTimer_;

	Texture2D* tex2D_;

	Vector2 timerPos_;

	//時計本体
	std::unique_ptr<Tex2DState> timerState_;
	//時計の針
	std::unique_ptr<Tex2DState> clockHandsState_;
	//時計の動く針
	std::unique_ptr<Tex2DState> moveClockHandsState_;

	//1の位
	uint32_t timeNumberNum_;
	//10の位
	uint32_t timeNumberTenNum_;
	//100の位
	uint32_t timeNumberHundredNum_;

	//1の位のテクスチャ
	std::unique_ptr<Tex2DState> timeNumberState_;
	//10の位のテクスチャ
	std::unique_ptr<Tex2DState> timeNumberTenState_;
	//100の位のテクスチャ
	std::unique_ptr<Tex2DState> timeNumberHundredState_;

	bool isFirstLoad_ = true;


};