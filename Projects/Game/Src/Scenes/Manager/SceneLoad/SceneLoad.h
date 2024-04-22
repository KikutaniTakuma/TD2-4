#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <memory>
#include "Drawers/Texture2D/Texture2D.h"
#include "Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"
#include "Utils/SafePtr/SafePtr.h"


// ロード中の描画クラス
class SceneLoad {
public:
	struct Desc {
		std::string fileName = "./Resources/Load.png";
		uint32_t animationNumber = 4;
		float animationSpeed = 0.5f;
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

	bool exit_;

	bool isLoad_;

	bool isWait_;

	std::unique_ptr<Tex2DAniamtor> tex2Danimator_;
	Lamb::SafePtr<Texture2D> loadTex_;
	uint32_t textureID_;

	Mat4x4 cameraMatrix_;

	class RenderContextManager* renderContextManager_;
};