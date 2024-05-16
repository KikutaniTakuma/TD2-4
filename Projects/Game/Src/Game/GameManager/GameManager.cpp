#include "GameManager.h"

#include <fstream>

#include "Drawers/DrawerManager.h"
#include "GameObject/Component/FallingBlockSpawnerComp.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include "Utils/SafePtr/SafePtr.h"
#include <Game/CollisionManager/Capsule/Capsule.h>
#include <Game/CollisionManager/Collider/Collider.h>
#include <GameObject/Component/DwarfAnimator.h>
#include <GameObject/Component/DwarfComp.h>
#include <GameObject/Component/DwarfShakeComp.h>
#include <GameObject/Component/FallingBlockComp.h>
#include <GameObject/Component/ModelComp.h>
#include <GameObject/Component/PlayerBulletComp.h>
#include <GameObject/Component/PlayerComp.h>
#include <GameObject/Component/Rigidbody.h>
#include <GameObject/Component/SpriteComp.h>
#include <GlobalVariables/GlobalVariables.h>
#include <GameObject/Component/EnemyBulletComp.h>

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

	spawner_ = std::make_unique<GameObject>();
	/*{
		Lamb::SafePtr modelComp = player_->AddComponent<ModelComp>();
		modelComp->AddBone("Body", drawerManager->GetModel("Resources/Cube.obj"));
	}*/
	{
		Lamb::SafePtr spriteComp = spawner_->AddComponent<SpriteComp>();
		spriteComp->SetTexture("./Resources/uvChecker.png");
		spriteComp->CalcTexUv();
	}

	{
		Lamb::SafePtr playerComp = spawner_->AddComponent<FallingBlockSpawnerComp>();
		playerComp->SetGauge(blockGauge_.get());
	}

	player_ = std::make_unique<GameObject>();
	player_->AddComponent<PlayerComp>();

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

	GetMap()->SetHitMap({});

	blockMap_->Update(deltaTime);
	blockBreakTimer_.Update(deltaTime);

	float localDeltaTime = deltaTime;

	if (blockBreakTimer_.IsActive()) {
		localDeltaTime *= blockBreakTimer_.GetProgress();
	}
	//if (not blockBreakTimer_.IsActive()) {
	blockMap_->Update(localDeltaTime);

	dwarfSpawnTimer_.Update(localDeltaTime);
	RandomDwarfSpawn();
	fallBlockSpawnTimer_.Update(localDeltaTime);
	RandomFallBlockSpawn();

	// 浮いているブロックを落とす
	BlockMapDropDown();

	MargeDwarf();

	player_->Update(localDeltaTime);

	spawner_->Update(localDeltaTime);
	for (auto &bullet : plBulletList_) {
		bullet->Update(localDeltaTime);
	};
	for (auto &bullet : enemyBulletList_) {
		bullet->Update(localDeltaTime);
	}
	for (auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Update(localDeltaTime);
	}

	for (auto &dwarf : dwarfList_) {
		dwarf->Update(localDeltaTime);
	}

	for (auto &dwarf : darkDwarfList_) {
		dwarf->Update(localDeltaTime);
	}

	for (auto &fallingBlock : fallingBlocks_) {
		// 落下しているブロックのコンポーネント
		Lamb::SafePtr fallComp = fallingBlock->GetComponent<FallingBlockComp>();
		// 落下しているブロックの座標
		Lamb::SafePtr blockBody = fallComp->pLocalPos_;

		if (fallingBlock->GetActive()) {
			Lamb::SafePtr playerBody = player_->GetComponent<LocalBodyComp>();
			const Vector2 centorDiff = blockBody->localPos_ - playerBody->localPos_;
			const Vector2 sizeSum = (blockBody->size_ + playerBody->size_) / 2.f;
			if (std::abs(centorDiff.x) <= sizeSum.x and std::abs(centorDiff.y) <= sizeSum.y) {
				fallingBlock->OnCollision(player_.get());
				player_->OnCollision(fallingBlock.get());
				player_->GetComponent<LocalRigidbody>()->ApplyInstantForce(Vector2{ SoLib::Math::Sign(centorDiff.x) * -2.f, 2.f });
			}
		}

		// もしブロックがあったら
		if (fallComp->IsLanding()) {
			Audio* audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/putBlock.mp3");
			audio->Start(0.2f, false);
			blockMap_->SetBlocks(blockBody->localPos_, blockBody->size_, fallComp->blockType_.GetBlockType());
			fallingBlock->SetActive(false);
		}
	}

	for (auto &bullet : enemyBulletList_) {
		if (bullet->GetActive()) {

			Lamb::SafePtr bulletBody = bullet->GetComponent<LocalBodyComp>();


			Lamb::SafePtr playerBody = player_->GetComponent<LocalBodyComp>();
			const Vector2 centorDiff = bulletBody->localPos_ - playerBody->localPos_;
			const Vector2 sizeSum = (bulletBody->size_ + playerBody->size_) / 2.f;
			if (std::abs(centorDiff.x) <= sizeSum.x and std::abs(centorDiff.y) <= sizeSum.y) {
				bullet->OnCollision(player_.get());
				player_->OnCollision(bullet.get());

			}
		}
	}

	// 落下ブロックの破棄
	std::erase_if(fallingBlocks_, [](const auto &itr) -> bool { return not itr->GetActive(); });
	std::erase_if(plBulletList_, [](const auto &itr) -> bool { return not itr->GetActive(); });
	std::erase_if(enemyBulletList_, [](const auto &itr) -> bool { return not itr->GetActive(); });

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
		for (auto dwarfItr = darkDwarfList_.begin(); dwarfItr != darkDwarfList_.end();) {
			Lamb::SafePtr dwarfObject = dwarfItr->get();

			// もし死んでいたら消す
			if (not dwarfObject->GetActive()) {

				dwarfObject->GetComponent<PickUpComp>()->ThrowAllBlocks();
				// 描画用にデータを渡す
				gameEffectManager_->dwarfDeadPos_.push_back(dwarfObject->GetComponent<LocalBodyComp>()->localPos_);

				dwarfItr = darkDwarfList_.erase(dwarfItr); // オブジェクトを破棄してイテレータを変更
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
		blockGauge_->EnergyRecovery(localDeltaTime);
	}

	blockGauge_->Update(localDeltaTime);
	//}
	gameEffectManager_->fallingBlock_ = spawner_->GetComponent<FallingBlockSpawnerComp>()->GetFutureBlockPos();

	gameEffectManager_->Update(deltaTime);

	// AABBのデータを転送
	blockMap_->TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	blockMap_->Draw(camera);
	player_->Draw(camera);
	spawner_->Draw(camera);
	for (const auto &bullet : plBulletList_) {
		bullet->Draw(camera);
	}
	for (const auto &bullet : enemyBulletList_) {
		bullet->Draw(camera);
	}
	for (const auto &fallingBlock : fallingBlocks_) {
		fallingBlock->Draw(camera);
	}
	for (const auto &dwarf : dwarfList_) {
		dwarf->Draw(camera);
	}
	for (const auto &dwarf : darkDwarfList_) {
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

GameObject *GameManager::AddPlayerBullet(Vector2 centerPos, Vector2 velocity)
{
	std::unique_ptr<GameObject> bullet = std::make_unique<GameObject>();

	bullet->AddComponent<PlayerBulletComp>();
	Lamb::SafePtr localBodyComp = bullet->AddComponent<LocalBodyComp>();

	localBodyComp->localPos_ = centerPos;
	localBodyComp->size_ = Vector2::kIdentity * 0.5f;
	bullet->AddComponent<LocalRigidbody>()->SetVelocity(velocity);
	plBulletList_.push_back(std::move(bullet));

	return plBulletList_.back().get();
}

GameObject *GameManager::AddEnemyBullet(Vector2 centerPos, Vector2 velocity)
{
	std::unique_ptr<GameObject> bullet = std::make_unique<GameObject>();

	bullet->AddComponent<EnemyBulletComp>();
	Lamb::SafePtr localBodyComp = bullet->AddComponent<LocalBodyComp>();

	localBodyComp->localPos_ = centerPos;
	localBodyComp->size_ = Vector2::kIdentity * 0.5f;
	bullet->AddComponent<LocalRigidbody>()->SetVelocity(velocity);
	enemyBulletList_.push_back(std::move(bullet));

	return enemyBulletList_.back().get();
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
	fallingComp->gravity_ = gravity;
	addBlock->GetComponent<LocalRigidbody>()->SetVelocity(velocity);

	// 末尾に追加
	fallingBlocks_.push_back(std::move(addBlock));

	// 参照を返す
	return fallingBlocks_.back().get();
}

void GameManager::LandBlock(Vector2 centerPos, Vector2 size, bool hasDamage)
{

	// ダメージ判定があるならダメージ判定を追加
	if (hasDamage) {
		DamageArea damage;
		// ブロックの下側にダメージ判定を出す
		damage.centerPos_ = centerPos - Vector2::kYIdentity * (size.y * 0.5f);
		// ダメージの横幅を設定する
		damage.size_.x = size.x;

		damageAreaList_.push_back(damage);

		// 演出用にデータを渡す
		gameEffectManager_->blockBreakPos_.push_back({ centerPos, size });

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
	Lamb::SafePtr dwarfComp = dwarf->AddComponent<DwarfComp>();
	Lamb::SafePtr localBody = dwarf->GetComponent<LocalBodyComp>();
	localBody->localPos_ = centerPos; // 座標の指定
	localBody->drawScale_ = 1.f;

	dwarf->AddComponent<DwarfAnimatorComp>();
	dwarf->AddComponent<DwarfShakeComp>();

	dwarfComp->updateFunc_.push_back(&DwarfComp::ChangeMovementTarget);

	// 末尾に追加
	dwarfList_.push_back(std::move(dwarf));
	// 参照を返す
	return dwarfList_.back().get();
}

GameObject *GameManager::AddDarkDwarf(Vector2 centerPos)
{
	// 小人の実体を構築
	std::unique_ptr<GameObject> dwarf = std::make_unique<GameObject>();

	// コンポーネントの追加
	Lamb::SafePtr dwarfComp = dwarf->AddComponent<DwarfComp>();
	Lamb::SafePtr localBody = dwarf->GetComponent<LocalBodyComp>();
	localBody->localPos_ = centerPos; // 座標の指定
	localBody->drawScale_ = 1.f;

	dwarf->AddComponent<DwarfAnimatorComp>();
	dwarf->AddComponent<DwarfShakeComp>();

	dwarfComp->updateFunc_.push_back(&DwarfComp::FireBullet);

	// 末尾に追加
	darkDwarfList_.push_back(std::move(dwarf));
	// 参照を返す
	return darkDwarfList_.back().get();
}


std::array<std::bitset<BlockMap::kMapX>, BlockMap::kMapY> &&GameManager::BreakChainBlocks(POINTS localPos)
{
	if (auto block = Block{ blockMap_->GetBlockType(localPos) }; block) {

		blockBreakTimer_.Start(vBreakStopTime_);

	}
	auto dwarfPos = GetDwarfPos();

	auto &&chainBlockMap = blockMap_->FindChainBlocks(localPos, dwarfPos);

	POINTS targetPos{};

	for (targetPos.y = 0; targetPos.y < BlockMap::kMapY; targetPos.y++) {
		const auto &breakLine = chainBlockMap[targetPos.y];
		for (targetPos.x = 0; targetPos.x < BlockMap::kMapX; targetPos.x++) {
			if (breakLine[targetPos.x]) {
				blockMap_->BreakBlock(targetPos);
			}
		}
	}

	for (auto &dwarf : dwarfList_) {
		auto localBody = dwarf->GetComponent<LocalBodyComp>();
		if (localBody) {
			POINTS mapIndex = localBody->GetMapPos();
			// そこが破壊対象なら死ぬ
			if (chainBlockMap[mapIndex.y][mapIndex.x]) {
				Audio* audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/slimeDeath.mp3");
				audio->Start(0.2f, false);

				dwarf->SetActive(false);
			}
		}
	}

	blockMap_->SetBreakMap(chainBlockMap);

	return std::move(chainBlockMap);
}
std::array<std::bitset<BlockMap::kMapX>, BlockMap::kMapY> &&GameManager::HitChainBlocks(POINTS localPos) {

	if (auto block = Block{ blockMap_->GetBlockType(localPos) }; block) {
		GetMap()->SetDamageColor(block.GetColor());
	}

	auto dwarfPos = GetDwarfPos();

	auto &&chainBlockMap = blockMap_->FindChainBlocks(localPos, dwarfPos);

	GetMap()->SetHitMap(chainBlockMap);

	return std::move(chainBlockMap);
}
void GameManager::RandomDwarfSpawn()
{
	if (not dwarfSpawnTimer_.IsActive()) {
		dwarfSpawnTimer_.Start(5.f);
		int32_t spawnPos = Lamb::Random(0, BlockMap::kMapX);
		AddDwarf(Vector2{ static_cast<float>(spawnPos), 0 });
	}
}
void GameManager::RandomFallBlockSpawn()
{
	if (not fallBlockSpawnTimer_.IsActive()) {
		fallBlockSpawnTimer_.Start(5.f);
		int32_t spawnPos = Lamb::Random(0, BlockMap::kMapX);
		uint32_t blockType = Lamb::Random(static_cast<uint32_t>(Block::BlockType::kNone) + 1, static_cast<uint32_t>(Block::BlockType::kMax) - 1);


		AddFallingBlock(Vector2{ static_cast<float>(spawnPos), static_cast<float>(BlockMap::kMapY) }, Vector2::kIdentity, static_cast<Block::BlockType>(blockType), Vector2::kYIdentity * -10, Vector2::kZero);
	}
}
//
// std::pair<PickUpBlockData, Vector2> GameManager::PickUp(Vector2 localPos, [[maybe_unused]] int hasBlockWeight, [[maybe_unused]] int maxWeight, [[maybe_unused]] bool isPowerful)
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

std::unordered_set<POINTS> GameManager::GetDwarfPos() const
{
	std::unordered_set<POINTS> result;

	result.reserve(dwarfList_.size() + darkDwarfList_.size());

	for (const auto &dwarf : dwarfList_) {
		// ドワーフの体コンポーネントを取得する
		Lamb::SafePtr body = dwarf->GetComponent<LocalBodyComp>();

		// ドワーフのマップ上の座標を取得する
		POINTS pos = body->GetMapPos();

		// データを格納する
		result.insert(pos);
	}
	for (const auto &dwarf : darkDwarfList_) {
		// ドワーフの体コンポーネントを取得する
		Lamb::SafePtr body = dwarf->GetComponent<LocalBodyComp>();

		// ドワーフのマップ上の座標を取得する
		POINTS pos = body->GetMapPos();

		// データを格納する
		result.insert(pos);
	}

	return result;
}

void GameManager::InputAction()
{
	{
		// スポナーのコンポーネント
		Lamb::SafePtr spawnerComp = spawner_->GetComponent<FallingBlockSpawnerComp>();
		Lamb::SafePtr key = input_->GetKey();

		// DIK_DOWN を押したときに実行
		if (key->Pushed(DIK_DOWN)) {
			spawnerComp->SetStartPos(); // 落下開始地点を設定
		}
		if (key->Released(DIK_DOWN)) {
			spawnerComp->SpawnFallingBlock(); // 落下ブロックを追加
		}

		if (key->Pushed(DIK_UP)) {
			spawnerComp->fallBlockType_ = static_cast<Block::BlockType>(static_cast<uint32_t>(spawnerComp->fallBlockType_.GetBlockType()) + 1);
			if (not spawnerComp->fallBlockType_) {
				spawnerComp->fallBlockType_ = Block::BlockType::kRed;
			}
		}

		// スポナーに付与する移動方向
		int32_t inputSpawner{};

		// 横方向の移動
		inputSpawner -= key->Pushed(DIK_LEFT);
		inputSpawner += key->Pushed(DIK_RIGHT);

		spawnerComp->MoveInput(inputSpawner);
	}
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
				blockMap_->GetBlockStatusMap()->at(static_cast<int32_t>(localPos.y)).at(static_cast<int32_t>(localPos.x)).reset();
			}

			// 最後に加算
			index++;
			localPos.x++;
		}
		localPos.y++;
	}
}

void GameManager::MargeDwarf()
{
	if (not dwarfList_.empty()) {
		for (decltype(dwarfList_)::iterator fItr = dwarfList_.begin(); fItr != dwarfList_.end(); ++fItr) {
			auto *const fDwarf = (*fItr).get();
			// 死んでたら飛ばす
			if (not fDwarf->GetActive()) { continue; }
			Lamb::SafePtr fDwComp = fDwarf->GetComponent<DwarfComp>();
			Lamb::SafePtr fBody = fDwarf->GetComponent<LocalBodyComp>();

			for (decltype(dwarfList_)::iterator sItr = std::next(fItr); sItr != dwarfList_.end(); ++sItr) {
				auto *const sDwarf = (*sItr).get();
				// 死んでたら飛ばす
				if (not sDwarf->GetActive()) { continue; }
				Lamb::SafePtr sDwComp = sDwarf->GetComponent<DwarfComp>();
				Lamb::SafePtr sBody = sDwarf->GetComponent<LocalBodyComp>();

				// 向いている向きが一致しない場合
				if (sDwComp->GetFacing() != fDwComp->GetFacing()) {

					const Vector2 centerDiff = fBody->localPos_ - sBody->localPos_;
					const Vector2 sizeSum = (fBody->size_ + sBody->size_) / 2.f;
					if (std::abs(centerDiff.x) <= sizeSum.x and std::abs(centerDiff.y) <= sizeSum.y) {

						fDwarf->SetActive(false);
						sDwarf->SetActive(false);

						// 召喚先
						Vector2 spawnPos = fBody->localPos_;

						AddDarkDwarf(spawnPos);

					}
				}

			}
		}
	}
}
