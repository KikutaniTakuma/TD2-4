#include "GameManager.h"
#include "GameObject/Component/IvyComponent.h"

void GameManager::Init() {
	GameObject *monoIvy = ivys_.emplace_back(std::make_unique<GameObject>()).get();
	monoIvy->AddComponent<IvyComponent>();


	input_ = Input::GetInstance();
}

void GameManager::Update([[maybe_unused]] const float deltaTime) {
	for (auto &ivy : ivys_) {
		ivy->Update(deltaTime);
	}
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const {
	for (const auto &ivy : ivys_) {
		ivy->Draw(camera);
	}
}

void GameManager::InputFunc() {
	// ツタが 1以上あれば
	if (ivys_.size()) {
		// A を押したときに実行
		if (input_->GetGamepad()->GetButton(Gamepad::Button::A)) {
			// 末尾のツタ
			auto ivy = ivys_.back().get();

			auto ivyComp =ivy->GetComponent<IvyComponent>();

		}
	}
}
