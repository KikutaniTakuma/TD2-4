#include "Framework.h"
#include "Engine/Engine.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Engine/Graphics/ResourceManager/ResourceManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Error/Error.h"

#include "Input/Input.h"

#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Line/Line.h"
#include "Drawers/Particle/Particle.h"

#include "Math/Vector2.h"

void Framework::Initialize() {
	ResourceManager::Initialize();
	// ライブラリ初期化
	Engine::Initialize(initDesc_.windowName, initDesc_.windowSize, initDesc_.maxFps, initDesc_.isFullesceen);


	// 入力処理初期化
	Input::Initialize();

	// 各種パイプライン生成
	Texture2D::Initialize();
	Mesh::Initialize();
	Model::Initialize();
	Line::Initialize();
	Particle::Initialize();
}

void Framework::Finalize() {
	// インデックスリソース解放
	Line::Finalize();
	Particle::Finalize();
	Texture2D::Finalize();

	// 入力関連解放
	Input::Finalize();

	ResourceManager::Finalize();

	// ライブラリ終了
	Engine::Finalize();
}

void Framework::Execution() {
	try {
		this->Initialize();

		FrameInfo* const frameInfo = FrameInfo::GetInstance();
		Input* const input = Input::GetInstance();
		WindowFactory* const window = WindowFactory::GetInstance();

		while (window->WindowMassage()) {
			// 描画開始処理
			Engine::FrameStart();

			// fps
			frameInfo->Debug();

			// 入力処理
			input->InputStart();

			// フルスクリーン化
			window->Fullscreen();

#ifdef _DEBUG		
			if (frameInfo->GetIsDebugStop() && frameInfo->GetIsOneFrameActive()) {
				this->Update();
				frameInfo->SetIsOneFrameActive(false);
			}
			else if (!frameInfo->GetIsDebugStop()) {
				// 更新処理
				this->Update();
			}
#else
			// 更新処理
			this->Update();
#endif
			// 終了リクエストが来たら終わり
			if (this->isEnd_) {
				break;
			}

			// 描画処理
			this->Draw();

			Line::AllDraw();

			// フレーム終了処理
			Engine::FrameEnd();
		}
	}
	catch (const Lamb::Error& err) {
		Lamb::ErrorLog(err);
	}
	catch (const std::exception& err) {
		Lamb::ErrorLog(err.what(), __func__);
	}
	this->Finalize();
}