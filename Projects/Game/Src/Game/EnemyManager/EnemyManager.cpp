#include "EnemyManager.h"
EnemyManager::~EnemyManager() {

}
void EnemyManager::Initialize() {
	input_ = Input::GetInstance();

	map_ = GameManager::GetInstance()->GetMap();

	mapSize_ = map_->GetBlockMap();

}

void EnemyManager::Finalize() {
	startPosList_.clear();
	enemyList_.clear();
}

void EnemyManager::Update(float deltaTime) {
	enemyList_.remove_if([&](const std::unique_ptr<GameObject> &enemy) {
		if (not enemy->GetActive()) {
			return true;
		}
		return false;
		});


	// 箱の数分のモデルを更新する
	for (auto &enemy : enemyList_) {
		enemy->Update(deltaTime);
	}

}

void EnemyManager::Draw(const Camera &camera) {
	std::list<std::unique_ptr<GameObject>>::iterator enemyItr = enemyList_.begin();

	// 箱の数分のモデルを描画する
	for (const auto &enemy : enemyList_) {
		enemy->Draw(camera);
	}
}

void EnemyManager::Debug() {
	ImGui::Begin("エネミー関連");
	ImGui::Text("エネミーの数 %d", static_cast<int>(startPosList_.size()));
	static auto enemyItr = enemyList_.begin();
	int32_t index = 0;
	SoLib::ImGuiWidget("エネミーのリスト", &enemyList_, enemyItr, [&index](decltype(enemyList_)::iterator itr)->std::string {return std::to_string(index++); });
	if (enemyItr != enemyList_.end()) {
		(*enemyItr)->ImGuiWidget();
	}

	ImGui::End();
}

void EnemyManager::AddEnemy(const Vector3 &pos) {
	newEnemy_ = std::make_unique<GameObject>();
	newEnemy_->transform_.translate = pos;
	startPosList_.push_back(pos);

	enemyList_.push_back(std::move(newEnemy_));
}

void EnemyManager::DeadEnemy(const Vector3 &) {
	//std::list<std::unique_ptr<GameObject>>::iterator enemyItr = enemyList_.begin();

	//// 箱の数分のモデルを描画する
	//for (size_t i = 0; i < EnemyCount_; i++) {
	//	Vector3 deadPos = (*enemyItr)->GetStartPos();
	//	if (deadPos == pos) {
	//		startPosList_.erase(std::remove(startPosList_.begin(), startPosList_.end(), pos), startPosList_.end());
	//		(*enemyItr)->SetDead();
	//	}
	//	enemyItr++;
	//}
}

void EnemyManager::ListReset() {

	startPosList_.clear();
	enemyList_.clear();
}
