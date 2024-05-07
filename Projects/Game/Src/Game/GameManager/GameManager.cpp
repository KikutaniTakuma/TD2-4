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
#include <GameObject/Component/SpriteComp.h>
#include <GameObject/Component/DwarfAnimator.h>
#include <GameObject/Component/DwarfShakeComp.h>

void GameManager::Init()
{

	blockGauge_ = std::make_unique<BlockGauge>();
	blockGauge_->Init();

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<BlockMap>();
	blockMap_->Init();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("Resources/Cube.obj");

	// 参照を渡す
	LocalBodyComp::pGameManager_ = this;
	LocalBodyComp::pMap_ = GetMap();

	player_ = std::make_unique<GameObject>();
	/*{
		Lamb::SafePtr modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", drawerManager->GetModel("Resources/Cube.obj"));
	}*/
	{
		Lamb::SafePtr spriteComp = player_->AddComponent<SpriteComp>();
		spriteComp->SetTexture("./Resources/uvChecker.png");
		spriteComp->CalcTexUv();
	}

	{
		Lamb::SafePtr playerComp = player_->AddComponent<PlayerComp>();
		playerComp->SetGauge(blockGauge_.get());
	}

	for (float i = 0; i < 15.f; i++) {
		AddDwarf(Vector2::kXIdentity * i);
	}

	gameEffectManager_ = std::make_unique<GameEffectManager>();
	gameEffectManager_->Init();
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{
	// 演出用のデータの破棄
	gameEffectManager_->Clear();

	GlobalVariables::GetInstance()->Update();

	blockMap_->Update(deltaTime);
	BlockMapDropDown();

	player_->Update(deltaTime);
	for (auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Update(deltaTime);
	}

	for (auto &dwarf : dwarfList_) {
		dwarf->Update(deltaTime);
	}

	for (auto &fallingBlock : fallingBlocks_) {
		// 落下しているブロックのコンポーネント
		Lamb::SafePtr fallComp = fallingBlock->GetComponent<FallingBlockComp>();
		// 落下しているブロックの座標
		Lamb::SafePtr blockBody = fallComp->pLocalPos_;

		//// 中心座標
		//Vector2 localCenter = blockBody->localPos_ + Vector2::kIdentity * 0.5f;

		//// 下の座標
		//Vector2 localDownPos = localCenter + Vector2{0, blockBody->size_.y / 2};

		// もしブロックがあったら
		if (fallComp->IsLanding()) {
			blockMap_->SetBlocks(blockBody->localPos_, blockBody->size_, fallComp->blockType_);
			fallingBlock->SetActive(false);
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
				// 描画用にデータを渡す
				gameEffectManager_->dwarfDeadPos_.push_back(dwarfObject->GetComponent<LocalBodyComp>()->localPos_);

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

	// ボタンを押していない間はゲージを回復する
	if (not Input::GetInstance()->GetKey()->GetKey(DIK_SPACE)) {
		blockGauge_->EnergyRecovery(deltaTime);
	}

	blockGauge_->Update(deltaTime);

	gameEffectManager_->fallingBlock_ = player_->GetComponent<PlayerComp>()->GetFutureBlockPos();

	gameEffectManager_->Update(deltaTime);

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
	blockGauge_->Draw(camera);

	gameEffectManager_->Draw(camera);

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


GameObject *GameManager::AddFallingBlock(Vector2 centerPos, Vector2 size, Block::BlockType blockType, Vector2 velocity, Vector2 gravity)
{
	std::unique_ptr<GameObject> addBlock = std::make_unique<GameObject>();

	// 落下するブロックのコンポーネント
	Lamb::SafePtr fallingComp = addBlock->AddComponent<FallingBlockComp>();
	Lamb::SafePtr localBodyComp = addBlock->AddComponent<LocalBodyComp>();

	localBodyComp->localPos_ = centerPos;
	localBodyComp->size_ = size;

	fallingComp->blockType_ = blockType;
	fallingComp->velocity_ = velocity;
	fallingComp->gravity_ = gravity;

	// 末尾に追加
	fallingBlocks_.push_back(std::move(addBlock));

	// 参照を返す
	return fallingBlocks_.back().get();
}

void GameManager::LandBlock(Vector2 centerPos, Vector2 size, bool hasDamage)
{


	// ダメージ判定があるならダメージ判定を追加
	if (hasDamage)
	{
		DamageArea damage;
		// ブロックの下側にダメージ判定を出す
		damage.centerPos_ = centerPos - Vector2::kYIdentity * (size.y * 0.5f);
		// ダメージの横幅を設定する
		damage.size_.x = size.x;

		damageAreaList_.push_back(damage);

		// 演出用にデータを渡す
		gameEffectManager_->blockBreakPos_.push_back({ centerPos,size });

	}
	else {
		// ブロックを設置
		blockMap_->SetBlocks(centerPos, size, Block::BlockType::kRed);
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
	localBody->drawScale_ = 1.f;

	dwarf->AddComponent<DwarfAnimatorComp>();
	dwarf->AddComponent<DwarfShakeComp>();

	// 末尾に追加
	dwarfList_.push_back(std::move(dwarf));
	// 参照を返す
	return dwarfList_.back().get();
}
//
//std::pair<PickUpBlockData, Vector2> GameManager::PickUp(Vector2 localPos, [[maybe_unused]] int hasBlockWeight, [[maybe_unused]] int maxWeight, [[maybe_unused]] bool isPowerful)
//{
//
//	// 範囲外である場合は終わる
//	if (BlockMap::IsOutSide(localPos)) { return {}; }
//	// ブロックが存在しない場合は終わる
//	if (blockMap_->GetBoxType(localPos) == Block::BlockType::kNone) { return {}; }
//
//	// ブロックのデータを取得する
//	Lamb::SafePtr groundBlock = (*blockMap_->GetBlockStatusMap())[static_cast<int32_t>(localPos.y)][static_cast<int32_t>(localPos.x)].get();
//
//	// もしそのデータが無い場合は終わる
//	if (groundBlock == nullptr) { return {}; }
//
//	// そのブロックの重さが上限値を超えてたら終わる
//	//if (hasBlockWeight + groundBlock->GetWeight() > maxWeight) { return {}; }
//
//	// ブロックのデータを取得する
//	// 中心座標を取得する
//	//Vector2 center = groundBlock->centerPos_;
//	//// ブロックの大きさを取得する
//	//Vector2 blockSize = groundBlock->blockSize_;
//
//
//	//// ブロックのデータをもとに、連結しているブロックを破壊する
//
//	//// 左下の座標を取得する
//	//Vector2 leftDownPos = center - (blockSize - Vector2::kIdentity) / 2.f;
//
//	//// 左上の座標
//	//Vector2 leftTopPos = leftDownPos + Vector2::kYIdentity * (blockSize.y - 1);
//
//
//	//// 左下からfor文で上にブロックがあるか調べる
//	//for (int x = 0; x < blockSize.x; x++)
//	//{
//	//	// 1つ上の調べるブロックの座標
//	//	Vector2 targetPos = Vector2{ leftTopPos.x + x, leftTopPos.y + 1 };
//	//	bool isExistsBlock = false;
//
//	//	// ブロックが存在したらフラグを立てる
//	//	isExistsBlock = blockMap_->GetBoxType(targetPos) == Map::BoxType::kGroundBlock;
//	//	// 上が虚空なら存在しない
//	//	isExistsBlock &= Map::IsOutSide(targetPos) == false;
//
//	//	// 存在したら
//	//	if (isExistsBlock && isPowerful == false)   // なおかつ､上にブロックがあっても持ち上げるフラグが折れていたら
//	//	{
//	//		// 持ち上げずに終わる
//	//		return {};
//	//	}
//
//	//}
//
//	//// 左下からfor文で破棄して、その座標のデータを壊す
//	//for (int yi = 0; yi < blockSize.y; yi++)
//	//{
//	//	for (int xi = 0; xi < blockSize.x; xi++)
//	//	{
//	//		// 壊す先のブロックの座標
//	//		Vector2 breakPos = { leftDownPos.x + xi, leftDownPos.y + yi };
//
//	//		int32_t yPos = static_cast<int32_t>(breakPos.y);
//	//		int32_t xPos = static_cast<int32_t>(breakPos.x);
//
//	//		// データを壊す
//	//		blockMap_->GetBlockMap()->at(yPos).at(xPos) = Map::BoxType::kNone;
//	//		blockMap_->GetBlockStatusMap()->at(yPos).at(xPos).reset();
//	//	}
//	//}
//
//
//
//	// 返すブロックのデータ
//	PickUpBlockData result = {};
//	// ブロックを破壊した後のデータを返す
//	result.size_ = Vector2::kZero;
//
//	return { result,  Vector2::kZero };
//
//}

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

void GameManager::BlockMapDropDown()
{
	Vector2 localPos = {};
	// ブロックの二次元配列
	auto &blMap = *blockMap_->GetBlockMap();

	// ブロックが着地しているか
	std::bitset<BlockMap::kMapX> isFloatBlock{};

	for (auto &blockLine : blMap) {
		uint32_t index = 0;
		localPos.x = 0;
		for (auto &block : blockLine) {

			// もしそこが虚空であればフラグを立てる
			if (not block) {
				isFloatBlock[index] = true;
			}
			// そこにブロックがあり、接地していない場合は虚空にして落下させる
			else if (isFloatBlock[index]) {
				AddFallingBlock(localPos, Vector2::kIdentity, block.GetBlockType(), Vector2::kYIdentity * -10, Vector2::kZero);
				block.SetBlockType(Block::BlockType::kNone);
			}


			// 最後に加算
			index++;
			localPos.x++;
		}
		localPos.y++;
	}


}
