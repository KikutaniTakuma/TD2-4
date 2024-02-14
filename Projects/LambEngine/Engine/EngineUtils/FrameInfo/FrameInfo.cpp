#include "FrameInfo.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Input/KeyInput/KeyInput.h"


#include <cmath>
#include <thread>
#include <Windows.h>
#undef max
#undef min
#include <fstream>
#include <string>
#include <format>
#include <algorithm>

FrameInfo::FrameInfo() :
	kMaxMonitorFps_(GetMainMonitorFramerate()),
#ifdef _DEBUG
	isDebugStopGame_(false),
	isOneFrameActive_(false),
	isFixedDeltaTime_(false),
#endif // _DEBUG
	frameStartTime_(),
	deltaTime_(0.0),
	fps_(0.0),
	maxFps_(0.0),
	minFps_(0.0),
	frameCount_(0),
	fpsLimit_(0.0),
	minTime_(),
	minCheckTime_(),
	gameSpeedSccale_(1.0),
	frameDatas_(),
	frameDataDuration_(1),
	frameDataDurationStartTime_{},
	avgProcDuration_{600llu},
	fpsStringOutPut_{},
	isDrawFps_(false)
{
	// リフレッシュレート取得
	fps_ = kMaxMonitorFps_;
	minFps_ = maxFps_ = fps_;
	deltaTime_ = 1.0 / fps_;

	auto nowTime = std::chrono::steady_clock::now();
	gameStartTime_ = nowTime;
	reference_ = nowTime;
	frameDataDurationStartTime_ = nowTime;

	maxFpsLimit_ = fps_;

	SetFpsLimit(fps_);

	fpsStringOutPut_.SetFormat("./Resources/Font/default.spritefont");
	fpsStringOutPut_.scale *= 0.3f;
}

FrameInfo::~FrameInfo() {
	auto end = std::chrono::steady_clock::now();

	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);


	maxFps_ = std::clamp(maxFps_, 0.0, fpsLimit_);
	minFps_ = std::clamp(minFps_, 0.0, fpsLimit_);

	double avgFps = 0.0;
	double size = static_cast<double>(frameDatas_.size());
	while (!frameDatas_.empty()) {
		avgFps += frameDatas_.front();

		frameDatas_.pop();
	}

	avgFps /= size;

	Lamb::AddLog(std::format("Frame count : {}", frameCount_));

	auto playtime =
		std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_);

	auto h = std::chrono::duration_cast<std::chrono::hours>(playtime);
	playtime -= h;
	auto m = std::chrono::duration_cast<std::chrono::minutes>(playtime);
	playtime -= m;
	auto s = playtime;

	Lamb::AddLog(std::format("Play time : {} {} {}", h,m,s));

	Lamb::AddLog(std::format("Average Fps : {:.2f}", avgFps));
	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		Lamb::AddLog(std::format("Max Fps : {:.2f}", maxFps_));
		Lamb::AddLog(std::format("Min Fps : {:.2f}", minFps_));
	}
}

FrameInfo* const FrameInfo::GetInstance() {
	static FrameInfo instance;

	return &instance;
}

void FrameInfo::Start() {
	frameStartTime_ = std::chrono::steady_clock::now();
}

void FrameInfo::End() {
	static std::chrono::steady_clock::time_point end{};
	end = std::chrono::steady_clock::now();

	// 10^-6
	static constexpr double unitAdjustment = 0.000001;
	
	auto elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(end - reference_);

	if (elapsed < minCheckTime_) {
		while (std::chrono::steady_clock::now() - reference_ < minTime_) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(250));
		}
	}

	end = std::chrono::steady_clock::now();
	auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - frameStartTime_);

	deltaTime_ = static_cast<double>(frameTime.count()) * unitAdjustment;
	fps_ = 1.0 / deltaTime_;


	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		maxFps_ = std::max(fps_, maxFps_);
		minFps_ = std::min(fps_, minFps_);
	}

	frameCount_++;
	auto nowTime = std::chrono::steady_clock::now();
	reference_ = nowTime;

	if (std::chrono::duration_cast<std::chrono::seconds>(frameDataDurationStartTime_ - nowTime) < frameDataDuration_) {
		frameDataDurationStartTime_ = nowTime;
		frameDatas_.push(fps_);

		if (avgProcDuration_ < frameDatas_.size()) {
			double avgFps = 0.0;
			double size = static_cast<double>(frameDatas_.size());
			while (!frameDatas_.empty()) {
				avgFps += frameDatas_.front();

				frameDatas_.pop();
			}

			avgFps /= size;
			frameDatas_.push(avgFps);
		}
	}
}

void FrameInfo::DrawFps() {
	if (isDrawFps_) {
		fpsStringOutPut_.Clear();
		fpsStringOutPut_ << "fps : " << static_cast<int32_t>(fps_);
		fpsStringOutPut_.Draw();
	}
}

void FrameInfo::SwitchDarwFlg() {
	if (KeyInput::GetInstance()->Pushed(DIK_F3)) {
		isDrawFps_ = !isDrawFps_;
	}
}

void FrameInfo::SetFpsLimit(double fpsLimit) {
	fpsLimit_ = std::clamp(fpsLimit, 10.0, maxFpsLimit_);
	           
	minTime_ = std::chrono::microseconds(uint64_t(1000000.0 / fpsLimit_));
	minCheckTime_ = std::chrono::microseconds(uint64_t(1000000.0 / (fpsLimit_ + (5.0f * (fpsLimit_ / 60.0f)))));
}

void FrameInfo::Debug() {
	this->SwitchDarwFlg();

#ifdef _DEBUG
	static float fpsLimit = static_cast<float>(fpsLimit_);
	fpsLimit = static_cast<float>(fpsLimit_);

	if (KeyInput::GetInstance()->Pushed(DIK_F9)) {
		isDebugStopGame_ = !isDebugStopGame_;
	}
	if (KeyInput::GetInstance()->Pushed(DIK_F11)) {
		if (isDebugStopGame_) {
			isOneFrameActive_ = true;
		}
	}

	ImGui::Begin("Frame Information");
	bool isThisWindowActive = WindowFactory::GetInstance()->IsThisWindowaActive();
	ImGui::Text(std::format("This Window Active : {}", isThisWindowActive).c_str());
	ImGui::Text("Frame rate : %3.0lf fps", fps_);
	ImGui::Text("Delta Time : %.4lf", deltaTime_);
	ImGui::Text("Frame Count : %llu", frameCount_);
	ImGui::DragFloat("fps limit", &fpsLimit, 1.0f, 1.0f, static_cast<float>(kMaxMonitorFps_));
	
	fpsLimit_ = static_cast<double>(fpsLimit);
	SetFpsLimit(fpsLimit_);
	if (ImGui::TreeNode("DEBUG")) {
		ImGui::Checkbox("is Debug Stop", &isDebugStopGame_);
		if (isDebugStopGame_) {
			if (ImGui::Button("DEBUG frame next")) {
				isOneFrameActive_ = true;
			}
		}
		else {
			ImGui::Checkbox("is fixed deltaTime", &isFixedDeltaTime_);
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

void FrameInfo::SetGameSpeedScale(float gameSpeedSccale) {
	gameSpeedSccale = std::clamp(gameSpeedSccale, 0.0f, 10.0f);
	gameSpeedSccale_ = static_cast<double>(gameSpeedSccale);
}

double FrameInfo::GetMainMonitorFramerate() const {
	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	// リフレッシュレート取得
	return static_cast<double>(mode.dmDisplayFrequency);
}
