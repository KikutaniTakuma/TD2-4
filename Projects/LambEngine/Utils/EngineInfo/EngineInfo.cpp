#include "EngineInfo.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Engine.h"

namespace Lamb {
	float DeltaTime() {
		static FrameInfo* const frameInfo = FrameInfo::GetInstance();
		return frameInfo->GetDelta();
	}

	Vector2 ClientSize() {
		static WindowFactory* const windowFactory = WindowFactory::GetInstance();
		return windowFactory->GetClientSize();
	}

	Vector2 WindowSize() {
		static WindowFactory* const windowFactory = WindowFactory::GetInstance();
		return windowFactory->GetWindowSize();
	}

	std::chrono::steady_clock::time_point ThisFrameTime() {
		static FrameInfo* const frameInfo = FrameInfo::GetInstance();
		return frameInfo->GetThisFrameTime();
	}

	bool IsEngineFianlize() {
		return Engine::IsFinalize();
	}
}