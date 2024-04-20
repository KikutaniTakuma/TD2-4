#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <memory>
#include "Drawers/Texture2D/Texture2D.h"
#include "Math/Mat4x4.h"


// ロード中の描画クラス
class SceneLoad {
public:
	struct Desc {
		std::string fileName = "./Resources/Load.png";
		uint32_t animationNumber = 4;
		uint32_t animationSpeed = 500;
	};
public:
	static Desc setting;

public:
	SceneLoad();
	SceneLoad(const SceneLoad&) = delete;
	SceneLoad(SceneLoad&&) = delete;
	~SceneLoad();

	SceneLoad& operator=(const SceneLoad&) = delete;
	SceneLoad& operator=(SceneLoad&&) = delete;

public:
	void Start();

	void Stop();

private:
	void CreateLoad();

private:
	// load描画用スレッド
	std::thread loadDrawThread_;
	// ロック用
	std::mutex mtx_;

	std::condition_variable condition_;

	// ロード中に実行する関数
	std::function<void(void)> loadProc_;
	std::unique_ptr<Texture2D> loadTex_;

	bool exit_;

	bool isLoad_;

	bool isWait_;

	Mat4x4 cameraMatrix_;

	class RenderContextManager* renderContextManager_;
};