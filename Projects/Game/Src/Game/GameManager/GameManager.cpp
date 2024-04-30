#include "GameManager.h"

#include <fstream>

#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GlobalVariables/GlobalVariables.h>
#include <GameObject/Component/ModelComp.h>
#include "Utils/SafePtr/SafePtr.h"
#include "Drawers/DrawerManager.h"
#include "GameObject/Component/PlayerComp.h"
#include <GameObject/Component/FallingBlockComp.h>
#include <GameObject/Component/Rigidbody.h>
#include <GameObject/Component/DwarfComp.h>

void GameManager::Init()
{

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<Map>();
	blockMap_->Init();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");

	// 参照を渡す
	LocalBodyComp::pGameManager_ = this;
	LocalBodyComp::pMap_ = GetMap();

	player_ = std::make_unique<GameObject>();
	{
		Lamb::SafePtr modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", drawerManager->GetModel("Resources/Cube.obj"));
	}

	{
		/*Lamb::SafePtr playerComp =*/ player_->AddComponent<PlayerComp>();
	}

	AddDwarf(Vector2::kZero);

}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	GlobalVariables::GetInstance()->Update();

	player_->Update(deltaTime);
	for (auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Update(deltaTime);
	}

	for (auto &dwarf : dwarfList_) {
		dwarf->Update(deltaTime);
	}

	// 落下ブロックの更新
	{
		for (auto blockItr = fallingBlocks_.begin(); blockItr != fallingBlocks_.end();) {
			Lamb::SafePtr fallBlock = blockItr->get();
			Lamb::SafePtr fallingComp = fallBlock->GetComponent<FallingBlockComp>();


			// もし着地してたら終わる
			if (fallingComp->IsLanding()) {

				// ローカル座標のコンポーネント
				const LocalBodyComp &localBodyComp = *fallingComp->pLocalPos_;

				// ブロックを設置
				LandBlock(localBodyComp.localPos_, localBodyComp.size_, fallingComp->hasDamage_);

				blockItr = fallingBlocks_.erase(blockItr); // オブジェクトを破棄してイテレータを変更
				continue;
			}


			// 何もなかったら追加
			++blockItr;
		}
	}


	// AABBのデータを転送
	blockMap_->TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	blockMap_->Draw(camera);
	player_->Draw(camera);
	for (const auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Draw(camera);
	}
	for (const auto &dwarf : dwarfList_) {
		dwarf->Draw(camera);
	}
}

bool GameManager::Debug([[maybe_unused]] const char *const str)
{

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	blockMap_->Debug("BlockMap");

	ImGui::End();

#endif // _DEBUG

	return isChange;
}


GameObject *GameManager::AddFallingBlock(Vector2 centerPos, Vector2 size, bool hasDamage, Vector2 velocity, Vector2 gravity)
{
	std::unique_ptr<GameObject> addBlock = std::make_unique<GameObject>();

	// 落下するブロックのコンポーネント
	Lamb::SafePtr fallingComp = addBlock->AddComponent<FallingBlockComp>();
	Lamb::SafePtr localBodyComp = addBlock->AddComponent<LocalBodyComp>();

	localBodyComp->localPos_ = centerPos;
	localBodyComp->size_ = size;

	fallingComp->hasDamage_ = hasDamage;
	fallingComp->velocity_ = velocity;
	fallingComp->gravity_ = gravity;

	// 末尾に追加
	fallingBlocks_.push_back(std::move(addBlock));

	// 参照を返す
	return fallingBlocks_.back().get();
}

void GameManager::LandBlock(Vector2 centerPos, Vector2 size, bool hasDamage)
{

	// ブロックを設置
	blockMap_->SetBlocks(centerPos, size, Map::BoxType::kGroundBlock);

	// ダメージ判定があるならダメージ判定を追加
	if (hasDamage)
	{
		DamageArea damage;
		// ブロックの下側にダメージ判定を出す
		damage.centerPos_ = centerPos - Vector2::kYIdentity * (size.y * 0.5f);
		// ダメージの横幅を設定する
		damage.size_.x = size.x;

		damageAreaList_.push_back(damage);

	}
}

GameObject *GameManager::AddDwarf(Vector2 centerPos)
{
	// 小人の実体を構築
	std::unique_ptr<GameObject> dwarf = std::make_unique<GameObject>();

	// コンポーネントの追加
	dwarf->AddComponent<DwarfComp>();
	Lamb::SafePtr localBody = dwarf->GetComponent<LocalBodyComp>();
	localBody->localPos_ = centerPos; // 座標の指定

	// 末尾に追加
	dwarfList_.push_back(std::move(dwarf));
	// 参照を返す
	return dwarfList_.back().get();
}

void GameManager::InputAction()
{
	// プレイヤのコンポーネント
	Lamb::SafePtr playerComp = player_->GetComponent<PlayerComp>();

	// SPACE を押したときに実行
	if (input_->GetKey()->Pushed(DIK_SPACE)) {
		playerComp->SetStartPos(); // 落下開始地点を設定
	}
	if (input_->GetKey()->Released(DIK_SPACE)) {
		playerComp->SpawnFallingBlock();	// 落下ブロックを追加
	}

	// プレイヤに付与する移動方向
	int32_t inputPlayer{};

	// 横方向の移動
	inputPlayer -= input_->GetKey()->Pushed(DIK_A);
	inputPlayer += input_->GetKey()->Pushed(DIK_D);

	playerComp->MoveInput(inputPlayer);

	// ベクトルの付与
	//player_->GetComponent<OnBlockMoveComp>()->InputMoveDirection(inputPlayer);

}