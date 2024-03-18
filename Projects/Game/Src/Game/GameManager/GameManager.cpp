#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/EnergyItem.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	struct InAABB {
		Vector2 min;
		Vector2 max;
	};

	GlobalVariables::GetInstance()->Update();

	IvyComponent::ApplyVariables("IvyComponent");
	IvyComponent::AddVariable("IvyComponent");

	for (auto &energy : energyItems_) {
		if (energy->GetComponent<EnergyItem>()->GetIsCollected()) {
			// 現在のツタが吸収した
			currentCollectedEnergyItems_.push_back(energy.get());
			// ツタの管理権を渡す
			collectedEnergyItems_.push_back(std::move(energy));
		}
	}

	std::erase_if(ivys_, [](auto &itr) -> bool { return not itr or not itr->GetActive(); });
	std::erase_if(energyItems_, [](auto &itr) -> bool { return not itr or not itr->GetActive(); });
	std::erase_if(collectedEnergyItems_, [](auto &itr) -> bool { return not itr or not itr->GetActive(); });

	// ツタが停止した場合
	if (not CurrentIvyIsActive()) {
		// ツタが最大値に達した場合破棄
		DeleteIvyMaximum();

		// メインのツタが存在する場合
		if (ivys_.size()) {
			// そのツタのに対してエネルギーを吸収
			AbsorbEnergy(ivys_.back()->GetComponent<IvyComponent>());
		}
	}

	for (auto &ivy : ivys_) {
		ivy->Update(deltaTime);
	}

	for (auto &energy : energyItems_) {
		energy->Update(deltaTime);
	}

	for (const auto &ivy : ivys_) {

		// ツタのコンポーネント
		IvyComponent *ivyComp = ivy->GetComponent<IvyComponent>();

		// そのツタが持っている全ての線
		const auto &ivyAllLines = ivyComp->GetAllLines();

		for (const auto &ivyLines : ivyAllLines) {
			for (const auto &line : *ivyLines) {
				for (auto &energy : energyItems_) {
					auto energyComp = energy->GetComponent<EnergyItem>();

					// 半径
					float rad = energyComp->GetRadius();
					if (Lamb::Collision::Capsule(line->start, line->end, rad, energy->transform_.translate, 0)) {
						// エネルギーの回収
						CollectEnergy(energy.get(), ivyComp);
					}
				}
			}
		}
	}
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	for (const auto &ivy : ivys_) {
		ivy->Draw(camera);
	}
	for (const auto &energy : energyItems_) {
		energy->Draw(camera);
	}
	// 掴んだやつを一旦表示しないように
	/*for (const auto &correctEnergy : collectedEnergyItems_) {
		correctEnergy->Draw(camera);
	}*/
}

bool GameManager::Debug([[maybe_unused]] const char *const str)
{

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	SoLib::ImGuiWidget(&centor_);
	SoLib::ImGuiWidget(&radius_);
	SoLib::ImGuiWidget(&count_);
	if (ImGui::Button("エネルギーをランダム生成")) {
		RandomPopEnergys(centor_, radius_, count_);
	}
	if (ImGui::Button("すべてのエネルギーを破棄")) {
		DeleteAllEnergy();
	}
	if (ImGui::Button("ツタを破棄")) {
		DeleteAllIvy();
	}
	if (ImGui::Button("すべてを破棄")) {
		DeleteAll();
	}

	if (ImGui::Button("エネルギーのデータを取得")) {

		std::ifstream file{ "./Resources/TestResource/energyPos.jsonc" };

		if (not file.fail()) {

			nlohmann::json root;

			file >> root;

			file.close();

			for (const auto &item : root["EnergyPos"]) {
				const Vector2 &pos = item;
				AddEnergy(pos);
			}

		}
	}

	SoLib::ImGuiText("分裂数", ivySplit_);
	SoLib::ImGuiText("ツタの長さ", ivyLength_);

	for (auto &ivy : ivys_) {
		SoLib::ImGuiText("ツタの世代数", ivy->GetComponent<IvyComponent>()->GetChildGeneration());
	}


	// SoLib::ImGuiWidget("Ivys", &ivys_, ivys_.begin(), [](const decltype(ivys_.begin()) &itr)->std::string { return SoLib::to_string((*itr)->GetComponent<IvyComponent>()->IsActive()); });

	ImGui::End();

#endif // _DEBUG

	return isChange;
}

void GameManager::InputAction()
{
	// A を押したときに実行
	if (input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		// ツタが 1以上あれば
		if (ivys_.size()) {

			// 分裂の実行
			bool successSplit = IvySprit();
			// 分裂に失敗していたら
			if (not successSplit) {
				// ツタを追加する
				RandomAddIvy();
			}
		}
		// ツタが無い場合
		else {
			// ツタの追加
			RandomAddIvy();
		}
	}
}

void GameManager::RandomPopEnergys(const Vector2 &origin, const Vector2 &radius, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		// 乱数による栄養の座標の算出
		Vector2 pos = origin + Lamb::Random(-radius, +radius);
		// 栄養アイテムを設置
		AddEnergy(pos);
	}
}

bool GameManager::IvySprit()
{
	// 末尾のツタ
	auto ivy = ivys_.back().get();
	// ツタのコンポーネントを取得
	auto ivyComp = ivy->GetComponent<IvyComponent>();

	// 分裂前の処理
	AbsorbEnergy(ivyComp); // エネルギーを吸収

	// 分裂に成功したか
	return ivyComp->SplitIvy(ivySplit_, ivyLength_); // 最大分岐数で分岐
}

void GameManager::DeleteIvyMaximum()
{
	// ツタが最大値を超えていた場合
	if (ivys_.size() >= maxIvyCount_) {
		// 最初のツタを破棄する
		DeleteIvy(ivys_.front().get());
	}
}

bool GameManager::CurrentIvyIsActive() const
{
	// ツタがある && 最後のツタが動いていた場合は true
	return not ivys_.empty() and ivys_.back()->GetComponent<IvyComponent>()->IsActive();
}

void GameManager::DeleteAllEnergy()
{
	energyItems_.clear();
}

void GameManager::DeleteAllIvy()
{
	ivys_.clear();
}

void GameManager::DeleteAll()
{
	DeleteAllEnergy();
	currentCollectedEnergyItems_.clear();
	collectedEnergyItems_.clear();
	ivys_.clear();
}

void GameManager::DeleteIvy(GameObject *ivy)
{
	IvyComponent *const ivyComp = ivy->GetComponent<IvyComponent>();

	// 生存フラグを消す
	ivy->SetActive(false);
	// 紐づいた座標を消す
	ivyPos_[ivyComp->GetPosIndex()] = false;
}

GameObject *GameManager::RandomAddIvy()
{
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

GameObject *GameManager::AddIvy(uint32_t index)
{
	GameObject *result = nullptr;

	// もしその値の場所にツタが存在しない場合は追加
	if (not ivyPos_[index]) {
		// 左から右にツタを追加していく
		result = AddIvy(Vector3{ (index - (ivyPos_.size() * 0.5f) + 0.5f) * vIvyDistance_, 0, 0 }, index);
		// フラグを立てる
		ivyPos_[index] = true;
	}

	return result;
}

GameObject *GameManager::AddIvy(const Vector3 &pos, uint32_t index)
{
	// 基礎分裂数を代入する
	ivySplit_ = vDefaultIvySplit_;
	// 基礎のツタの長さを代入
	ivyLength_ = vDefaultIvyLength_;

	// ツタのオブジェクト
	GameObject *monoIvy = nullptr;
	// ツタの数が最大値を超えない場合は追加
	if (ivys_.size() < maxIvyCount_) {
		// コンテナに追加して、それを返す
		monoIvy = ivys_.emplace_back(std::make_unique<GameObject>()).get();
		// 座標の変更
		monoIvy->transform_.translate = pos;
		// コンポーネントを追加
		IvyComponent *ivyComp = monoIvy->AddComponent<IvyComponent>();
		ivyComp->SetPosIndex(index); // indexを渡す
		ivyComp->SetIvyLength(ivyLength_);	// ツタの長さ
	}
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

void GameManager::CollectEnergy(GameObject *energy, IvyComponent *ivy)
{

	auto *const energyComp = energy->GetComponent<EnergyItem>();
	// 当たったエネルギーを回収する
	energyComp->SetIsCollected(true);
	energyComp->SetTargetIndex(ivy->GetPosIndex());
}

void GameManager::AddIvySplitCount(IvyComponent *ivy)
{

	// ツタの世代数
	uint32_t ivyGen = ivy->GetChildGeneration();

	// ツタの世代数が0以外の場合
	if (ivyGen != 0u) {

		// ツタの世代数の3倍の栄養を吸収していた場合、その分の分裂数を加算する
		size_t addSplitCount = currentCollectedEnergyItems_.size() / static_cast<size_t>(ivyGen * 3u);
		ivySplit_ += static_cast<uint32_t>(addSplitCount);
	}
}

void GameManager::ExtendIvyLength()
{

	// 一旦等倍で増えていく
	ivyLength_ += currentCollectedEnergyItems_.size() * *vExtendLength_;

}

void GameManager::AbsorbEnergy(IvyComponent *ivy) {

	ExtendIvyLength();
	AddIvySplitCount(ivy);
	currentCollectedEnergyItems_.clear(); // 前の分岐で獲得した栄養をリセット
}
