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

	for (float i = 0; i < 15.f; i++) {
		AddDwarf(Vector2::kXIdentity * i);
	}
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	GlobalVariables::GetInstance()->Update();

	blockMap_->Update(deltaTime);

	player_->Update(deltaTime);
	for (auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Update(deltaTime);
	}

	for (auto &dwarf : dwarfList_) {
		dwarf->Update(deltaTime);
	}


	for (auto &block : fallingBlocks_) {
		Lamb::SafePtr fallingComp = block->GetComponent<FallingBlockComp>();
		Lamb::SafePtr blockBody = fallingComp->pLocalPos_;

		for (auto &dwarf : dwarfList_) {

			// もし死んでいたらその時点で終わり
			if (not dwarf->GetActive()) { continue; }

			Lamb::SafePtr dwarfBody = dwarf->GetComponent<LocalBodyComp>();

			Lamb::SafePtr pickUpComp = dwarf->GetComponent<PickUpComp>();

			for (float yDiff = 1.f; const auto & pickUpBlock : pickUpComp->GetPickUpBlockList()) {

				Vector2 posDiff = dwarfBody->localPos_ - blockBody->localPos_ + Vector2::kYIdentity * (yDiff + (pickUpBlock.size_.y - 1) / 2);

				Vector2 halfSize = (pickUpBlock.size_ + blockBody->size_) * 0.5f;

				// ぶつかっていたら
				if (std::abs(posDiff.x) <= halfSize.x and std::abs(posDiff.y) <= halfSize.y) {

					// ブロックのデータを渡す
					pickUpComp->AddBlock({ .size_ = blockBody->size_ });

					// ブロックを破壊する
					block->SetActive(false);

					break;

				}


				// 高さを加算
				yDiff += pickUpBlock.size_.y;

			}
			// ブロックが壊れてたら飛ばす
			if (not block->GetActive()) { break; }

			Vector2 posDiff = dwarfBody->localPos_ - blockBody->localPos_;

			Vector2 halfSize = (dwarfBody->size_ + blockBody->size_) * 0.5f;

			//// ぶつかっていたら破壊
			//if (std::abs(posDiff.x) <= halfSize.x and std::abs(posDiff.y) <= halfSize.y) {

			//	dwarf->SetActive(false);

			//	break;

			//}

		}


		// ブロックが壊れてたら飛ばす
		if (not block->GetActive()) { continue; }

		// もし着地してたら
		if (fallingComp->IsLanding()) {
			// もし横に移動していたら
			if (fallingComp->velocity_.x != 0)
			{

				// 当たっていたのが敵拠点の時の処理

				// 当たった場所のリスト
				const auto &hitPosList = fallingComp->FindLandingList();
				for (Vector2 pos : hitPosList)
				{
					// もし､当たったブロックが敵拠点なら
					if (blockMap_->GetBoxType(pos) == Map::BoxType::kEnemyBlock)
					{
						//GameDataTransfar.blockHitAtTowerPos_.Add(Map::GetGrobalPos(pos));

						// 敵拠点を取得
						auto enemyHouse = blockMap_->GetNearestHouse(static_cast<int32_t>(pos.x));

						const int32_t xPos = enemyHouse.xPos_;
						Vector2 direction = Vector2::kXIdentity * SoLib::Math::Sign(fallingComp->velocity_.x);

						blockMap_->ProcessEnemyHouseBlocks([direction, xPos, this](int32_t y, int32_t x)
							{
								Vector2 blockFinder = { static_cast<float>(xPos + x), static_cast<float>(y) };
								if (blockMap_->GetBoxType(blockFinder) == Map::BoxType::kEnemyBlock)
								{
									Lamb::SafePtr ground = (*blockMap_->GetBlockStatusMap())[y][xPos + x].get();
									float power = y + 1.f;

									ground->StartShake(1.5f, direction * power);
								}
							});

						// 大きさ分のダメージを与える
						enemyHouse.health_ -= fallingComp->GetWeight();

						// 体力が0を下回ったら
						if (enemyHouse.health_ <= 0)
						{
							// 崩壊フラグを立てる
							enemyHouse.damageFacing_ = SoLib::Math::Sign(static_cast<int32_t>(fallingComp->velocity_.x));

						}
						break;

					}
				}


				// 移動量のメモ
				float xMove = fallingComp->velocity_.x;

				// 横移動を消してもう一度試す
				fallingComp->velocity_.x = 0;
				//isOnemore = true;
				// あたってなかったら
				if (fallingComp->IsLanding() == false)
				{
					// やり直す
					continue;
				}
				fallingComp->velocity_.x = xMove;
			}


			// ローカル座標のコンポーネント
			const LocalBodyComp &localBodyComp = *fallingComp->pLocalPos_;

			// ブロックを設置
			LandBlock(localBodyComp.localPos_, localBodyComp.size_, fallingComp->hasDamage_);
			// 破棄するように設定する
			block->SetActive(false);
		}
	}

	// 落下ブロックの破棄
	std::erase_if(fallingBlocks_, [](const auto &itr) ->bool { return not itr->GetActive(); });

	// 小人の破棄
	{
		for (auto dwarfItr = dwarfList_.begin(); dwarfItr != dwarfList_.end();) {
			Lamb::SafePtr dwarfObject = dwarfItr->get();


			// もし死んでいたら消す
			if (not dwarfObject->GetActive()) {

				dwarfObject->GetComponent<PickUpComp>()->ThrowAllBlocks();


				dwarfItr = dwarfList_.erase(dwarfItr); // オブジェクトを破棄してイテレータを変更
				continue;
			}


			// 何もなかったら次へ
			++dwarfItr;
		}
	}


	for (auto &dwarf : dwarfList_) {

		Lamb::SafePtr dwarfBody = dwarf->GetComponent<LocalBodyComp>();

		for (const auto &damageArea : damageAreaList_) {

			Vector2 posDiff = dwarfBody->localPos_ - damageArea.centerPos_;

			Vector2 size = (dwarfBody->size_ + damageArea.size_) * 0.5f;

			// ぶつかっていたら破壊
			if (std::abs(posDiff.x) <= size.x and std::abs(posDiff.y) <= size.y) {

				dwarf->SetActive(false);

				break;

			}


		}
	}

	damageAreaList_.clear();


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

PickUpBlockData GameManager::PickUp(Vector2 localPos, int hasBlockWeight, int maxWeight, bool isPowerful)
{

	// 範囲外である場合は終わる
	if (Map::IsOutSide(localPos)) { return {}; }
	// ブロックが存在しない場合は終わる
	if (blockMap_->GetBoxType(localPos) == Map::BoxType::kNone) { return {}; }

	// ブロックのデータを取得する
	Lamb::SafePtr groundBlock = (*blockMap_->GetBlockStatusMap())[static_cast<int32_t>(localPos.y)][static_cast<int32_t>(localPos.x)].get();

	// もしそのデータが無い場合は終わる
	if (groundBlock == nullptr) { return {}; }

	// そのブロックの重さが上限値を超えてたら終わる
	if (hasBlockWeight + groundBlock->GetWeight() > maxWeight) { return {}; }

	// ブロックのデータを取得する
	// 中心座標を取得する
	Vector2 center = groundBlock->centerPos_;
	// ブロックの大きさを取得する
	Vector2 blockSize = groundBlock->blockSize_;


	// ブロックのデータをもとに、連結しているブロックを破壊する

	// 左下の座標を取得する
	Vector2 leftDownPos = center - (blockSize - Vector2::kIdentity) / 2.f;

	// 左上の座標
	Vector2 leftTopPos = leftDownPos + Vector2::kYIdentity * (blockSize.y - 1);


	// 左下からfor文で上にブロックがあるか調べる
	for (int x = 0; x < blockSize.x; x++)
	{
		// 1つ上の調べるブロックの座標
		Vector2 targetPos = Vector2{ leftTopPos.x + x, leftTopPos.y + 1 };
		bool isExistsBlock = false;

		// ブロックが存在したらフラグを立てる
		isExistsBlock = blockMap_->GetBoxType(targetPos) == Map::BoxType::kGroundBlock;
		// 上が虚空なら存在しない
		isExistsBlock &= Map::IsOutSide(targetPos) == false;

		// 存在したら
		if (isExistsBlock && isPowerful == false)   // なおかつ､上にブロックがあっても持ち上げるフラグが折れていたら
		{
			// 持ち上げずに終わる
			return {};
		}

	}

	// 左下からfor文で破棄して、その座標のデータを壊す
	for (int yi = 0; yi < blockSize.y; yi++)
	{
		for (int xi = 0; xi < blockSize.x; xi++)
		{
			// 壊す先のブロックの座標
			Vector2 breakPos = { leftDownPos.x + xi, leftDownPos.y + yi };

			int32_t yPos = static_cast<int32_t>(breakPos.y);
			int32_t xPos = static_cast<int32_t>(breakPos.x);

			// データを壊す
			blockMap_->GetBlockMap()->at(yPos).at(xPos) = Map::BoxType::kNone;
			blockMap_->GetBlockStatusMap()->at(yPos).at(xPos).reset();
		}
	}



	// 返すブロックのデータ
	PickUpBlockData result = {};
	// ブロックを破壊した後のデータを返す
	result.size_ = blockSize;

	return result;

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