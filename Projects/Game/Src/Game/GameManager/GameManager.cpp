#include "GameManager.h"
#include "GameObject/Component/IvyComponent.h"

void GameManager::Init() {

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

void GameManager::InputAction() {
	// A を押したときに実行
	if (input_->GetGamepad()->GetButton(Gamepad::Button::A)) {
		// ツタが 1以上あれば
		if (ivys_.size()) {
			// 末尾のツタ
			auto ivy = ivys_.back().get();
			// ツタのコンポーネントを取得
			auto ivyComp = ivy->GetComponent<IvyComponent>();
			// 分裂に成功したか
			bool successSplit = ivyComp->SplitIvy(3);	// 最大分岐数 3 で分岐

			// 分裂に失敗していたら
			if (not successSplit) {
				// ツタを追加する
				AddIvy();
			}

		}
		//ツタが無い場合
		else {
			// ツタの追加
			AddIvy();
		}
	}
}

GameObject *GameManager::AddIvy() {
	// 追加したオブジェクトの名前
	GameObject *monoIvy = ivys_.emplace_back(std::make_unique<GameObject>()).get();
	// コンポーネントを追加
	monoIvy->AddComponent<IvyComponent>();
	return monoIvy;
}
