#include "GameManager.h"

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"

void GameManager::Init() {

	input_ = Input::GetInstance();
}

void GameManager::Update([[maybe_unused]] const float deltaTime) {
	for (auto &ivy : ivys_) {
		ivy->Update(deltaTime);
	}
	for (auto &energy : energyItems_) {
		energy->Update(deltaTime);
	}
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const {
	for (const auto &ivy : ivys_) {
		ivy->Draw(camera);
	}
	for (const auto &energy : energyItems_) {
		energy->Draw(camera);
	}
}

bool GameManager::Debug([[maybe_unused]] const char *const str) {

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	SoLib::ImGuiWidget(&centor_);
	SoLib::ImGuiWidget(&radius_);
	SoLib::ImGuiWidget(&count_);
	if (ImGui::Button("PopEnemy")) {
		RandomPopEnergys(centor_, radius_, count_);
	}

	ImGui::End();

#endif // _DEBUG

	return isChange;

}

void GameManager::InputAction() {
	// A を押したときに実行
	if (input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
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

void GameManager::RandomPopEnergys(const Vector2 &origin, const Vector2 &radius, size_t count) {
	for (size_t i = 0; i < count; i++) {
		// 乱数による栄養の座標の算出
		Vector2 pos = origin + Lamb::Random(-radius, +radius);

		// 追加する栄養アイテム
		std::unique_ptr<GameObject> newEnergy = std::make_unique<GameObject>();
		// 座標の指定
		newEnergy->transform_.translate = pos;
		// アイテムのコンポーネントを追加
		newEnergy->AddComponent<EnergyItem>();

		// コンテナに格納
		energyItems_.push_back(std::move(newEnergy));
	}
}


GameObject *GameManager::AddIvy() {
	// 追加したツタのオブジェクト
	GameObject *monoIvy = ivys_.emplace_back(std::make_unique<GameObject>()).get();
	// コンポーネントを追加
	monoIvy->AddComponent<IvyComponent>();
	return monoIvy;
}
