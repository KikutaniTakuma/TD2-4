#include "EnemyManager.h"
EnemyManager::~EnemyManager(){
	
}
void EnemyManager::Initialize(){
	input_ = Input::GetInstance();

	map_ = GameManager::GetInstance()->GetMap();

	mapSize_ = map_->GetBlockMap();

}

void EnemyManager::Finalize(){
	EnemyList_.clear();
}

void EnemyManager::Update(){
	EnemyList_.remove_if([&](const std::unique_ptr<Enemy>& enemy) {
		if (enemy->GetIsDead()) {
			EnemyCount_--;
			return true;
		}
		return false;
	});

	std::list<std::unique_ptr<Enemy>>::iterator enemyItr = EnemyList_.begin();
	
	
	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < EnemyCount_; i++) {		
		
		(*enemyItr++)->Update();	// 次のモデルに移動
	}

}

void EnemyManager::Draw(const Camera& camera){
	std::list<std::unique_ptr<Enemy>>::iterator enemyItr = EnemyList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < EnemyCount_; i++) {
		// モデルを描画
		(*enemyItr++)->Draw(camera);	// 次のモデルに移動
	}
}

void EnemyManager::Debug(){
	std::list<std::unique_ptr<Enemy>>::iterator enemyItr = EnemyList_.begin();
	ImGui::Begin("エネミー関連");
	ImGui::Text("エネミーの数 %d", static_cast<int>(EnemyCount_));
	if (EnemyCount_>0){
		for (size_t i = 0; i < EnemyCount_; i++) {
			(*enemyItr++)->Debug(i);
		}
	}
	
	ImGui::End();
}

void EnemyManager::AddEnemy(const Vector3& pos){
	newEnemy_ = std::make_unique<Enemy>();
	newEnemy_->Initialize();
	newEnemy_->pos_ = pos;

	EnemyList_.push_back(std::move(newEnemy_));
	EnemyCount_++;
}

void EnemyManager::DeadEnemy(const Vector3& pos){
	std::list<std::unique_ptr<Enemy>>::iterator enemyItr = EnemyList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < EnemyCount_; i++) {
		Vector3 deadPos = (*enemyItr)->GetPos();
		if (deadPos == pos) {
			(*enemyItr)->SetDead();
		}
		enemyItr++;
	}
}