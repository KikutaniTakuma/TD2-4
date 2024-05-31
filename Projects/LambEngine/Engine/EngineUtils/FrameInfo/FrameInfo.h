#pragma once
#include <chrono>
#include <queue>
#include "Drawers/StringOut/StringOut.h"

/// <summary>
/// フレーム情報の管理(fps固定、デルタタイム、平均fps)
/// </summary>
class FrameInfo {
/// <summary>
/// コンストラクタ
/// </summary>
private:
	FrameInfo();
	FrameInfo(const FrameInfo&) = delete;
	FrameInfo(FrameInfo&&) = delete;
	~FrameInfo();

	FrameInfo& operator=(const FrameInfo&) = delete;
	FrameInfo& operator=(FrameInfo&&) = delete;

/// <summary>
/// シングルトン
/// </summary>
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンスのポインタ(deleteしてはいけない)</returns>
	static FrameInfo* const GetInstance();

/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// フレーム開始時関数
	/// </summary>
	void Start();


	/// <summary>
	/// フレーム終了時関数
	/// </summary>
	void End();

/// <summary>
/// getter
/// </summary>
public:
	/// <summary>
	/// fps描画
	/// </summary>
	void DrawFps();

	/// <summary>
	/// fps描画切り替え
	/// </summary>
	void SwitchDarwFlg();

#ifdef _DEBUG
	bool GetIsDebugStop() const {
		return isDebugStopGame_;
	}

	bool GetIsOneFrameActive() const {
		return isOneFrameActive_;
	}

	void SetIsOneFrameActive(bool isOneFramActive) {
		if (isDebugStopGame_) {
			isOneFrameActive_ = isOneFramActive;
		}
	}
#endif // _DEBUG

	/// <summary>
	/// デバッグ関数
	/// </summary>
	void Debug();

/// <summary>
/// getter
/// </summary>
public:
	/// <summary>
	/// デルタタイム取得
	/// </summary>
	/// <returns>デルタタイム</returns>
	inline float GetDelta() const {
#ifdef _DEBUG
		if (isFixedDeltaTime_ || isDebugStopGame_) {
			return static_cast<float>(1.0 / fpsLimit_);
		}
		return static_cast<float>(deltaTime_);
#else
		return static_cast<float>(deltaTime_);
#endif
	}

	/// <summary>
	/// fps取得
	/// </summary>
	/// <returns>fps</returns>
	inline double GetFps() const {
		return fps_;
	}

	/// <summary>
	/// フレームカウント取得
	/// </summary>
	/// <returns>フレームカウント</returns>
	inline size_t GetFrameCount() const {
		return frameCount_;
	}

	/// <summary>
	/// ゲームスピード取得
	/// </summary>
	/// <returns></returns>
	inline float GetGameSpeedScale() const {
		return static_cast<float>(gameSpeedSccale_);
	}

	/// <summary>
	/// フレームの最初の時間を取得
	/// </summary>
	/// <returns>フレームの時間</returns>
	std::chrono::steady_clock::time_point GetThisFrameTime() const {
		return frameStartTime_;
	}

	/// <summary>
	/// メインモニターのリフレッシュレートを取得
	/// </summary>
	/// <returns>リフレッシュレート</returns>
	double GetMainMonitorFramerate() const;


/// <summary>
/// セッター
/// </summary>
public:
	/// <summary>
	/// fpsの上限値を設定(メインモニターのリフレッシュレートを超えることはない)
	/// </summary>
	/// <param name="fpsLimit">fps上限値</param>
	void SetFpsLimit(double fpsLimit);

	/// <summary>
	/// ゲームスピードのスケールを変更
	/// </summary>
	void SetGameSpeedScale(float gameSpeedSccale);



/// <summary>
/// メンバ変数
/// </summary>
private:
	const double kMaxMonitorFps_;

	std::chrono::steady_clock::time_point frameStartTime_;
	double deltaTime_;
	double fps_;
	double maxFps_;
	double minFps_;
	size_t frameCount_;

	std::chrono::steady_clock::time_point gameStartTime_;

	std::chrono::steady_clock::time_point reference_;
	double fpsLimit_;
	double maxFpsLimit_;

	std::chrono::microseconds minTime_;
	std::chrono::microseconds minCheckTime_;

	double gameSpeedSccale_;

	std::queue<double> frameDatas_;
	std::chrono::seconds frameDataDuration_;
	std::chrono::steady_clock::time_point frameDataDurationStartTime_;
	size_t avgProcDuration_;

	StringOut fpsStringOutPut_;
	bool isDrawFps_;
	

#ifdef _DEBUG
	bool isDebugStopGame_;
	bool isOneFrameActive_;
	bool isFixedDeltaTime_;
#endif // _DEBUG

};