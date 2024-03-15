#include "GameManager.h"

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Collider/Collider.h>
#include <Game/CollisionManager/Capsule/Capsule.h>

void GameManager::Init() {

	input_ = Input::GetInstance();
}

void GameManager::Update([[maybe_unused]] const float deltaTime) {
	struct InAABB {
		Vector2 min;
		Vector2 max;
	};

	std::erase_if(ivys_, [](auto &itr)->bool { return not itr->GetActive(); });
	std::erase_if(energyItems_, [](auto &itr)->bool { return not itr->GetActive(); });

	for (auto &ivy : ivys_) {
		ivy->Update(deltaTime);
		// 停止していた場合は削除
		DeleteIvy(ivy.get());
	}
	for (auto &energy : energyItems_) {
		energy->Update(deltaTime);
	}

	for (const auto &ivy : ivys_) {


		const auto &ivyAllLines = ivy->GetComponent<IvyComponent>()->GetAllLines();
		//InAABB aabb{ std::(ivyLines.front()->start.x), }
		for (const auto &ivyLines : ivyAllLines) {
			for (const auto &line : *ivyLines) {
				for (auto &energy : energyItems_) {
					// 半径
					float rad = energy->GetComponent<EnergyItem>()->GetRadius();
					if (Lamb::Collision::Capsule(line->start, line->end, rad, energy->transform_.translate, 0)) {
						energy->SetActive(false);
					}
				}
			}

		}
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

	// SoLib::ImGuiWidget("Ivys", &ivys_, ivys_.begin(), [](const decltype(ivys_.begin()) &itr)->std::string { return SoLib::to_string((*itr)->GetComponent<IvyComponent>()->IsActive()); });

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
				RandomAddIvy();
			}

		}
		//ツタが無い場合
		else {
			// ツタの追加
			RandomAddIvy();
		}
	}
}

void GameManager::RandomPopEnergys(const Vector2 &origin, const Vector2 &radius, size_t count) {
	for (size_t i = 0; i < count; i++) {
		// 乱数による栄養の座標の算出
		Vector2 pos = origin + Lamb::Random(-radius, +radius);
		// 栄養アイテムを設置
		AddEnergy(pos);
	}
}

void GameManager::DeleteIvy(GameObject *ivy)
{
	IvyComponent *const ivyComp = ivy->GetComponent<IvyComponent>();
	// もしツタが伸びてなかったら
	if (not ivyComp->IsActive()) {
		// 生存フラグを消す
		ivy->SetActive(false);
		// 紐づいた座標を消す
		ivyPos_[ivyComp->GetPosIndex()] = false;
	}
}

GameObject *GameManager::RandomAddIvy() {
	// ツタの場所から、選択されていない場所を選ぶ
	uint32_t unuseIndex = 0;
	for (uint32_t i = 0; i < static_cast<uint32_t>(ivyPos_.size()); i++) {
		// その場所が使われていたら飛ばす
		if (not ivyPos_[i]) {
			// 使われていない所であれば、それを返す
			unuseIndex = i;
			break;
		}
	}

	// ツタの追加
	return AddIvy(unuseIndex);
}

GameObject *GameManager::AddIvy(uint32_t index) {
	GameObject *result = nullptr;

	// もしその値の場所にツタが存在しない場合は追加
	if (not ivyPos_[index]) {
		// 左から右にツタを追加していく
		result = AddIvy(Vector3{ (index - (ivyPos_.size() * 0.5f) + 0.5f) * vIvyDistance_ , 0, 0 }, index);
		// フラグを立てる
		ivyPos_[index] = true;
	}

	return result;
}

GameObject *GameManager::AddIvy(const Vector3 &pos, uint32_t index)
{
	// コンテナに追加したツタのオブジェクト
	GameObject *monoIvy = ivys_.emplace_back(std::make_unique<GameObject>()).get();
	// 座標の変更
	monoIvy->transform_.translate = pos;
	// コンポーネントを追加
	monoIvy->AddComponent<IvyComponent>()->SetPosIndex(index);	// indexを渡す

	return monoIvy;
}

GameObject *GameManager::AddEnergy(const Vector3 &pos)
{
	// コンテナに追加した栄養アイテム
	GameObject *newEnergy = energyItems_.emplace_back(std::make_unique<GameObject>()).get();
	// 座標の変更
	newEnergy->transform_.translate = pos;
	// アイテムのコンポーネントを追加
	newEnergy->AddComponent<EnergyItem>();

	return newEnergy;
}

