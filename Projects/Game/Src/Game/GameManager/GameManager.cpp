#include "GameManager.h"

#include <fstream>

#include "Drawers/DrawerManager.h"
#include "GameObject/Component/FallingBlockSpawnerComp.h"
#include "GameObject/Component/IvyComponent.h"
#include "Utils/Random/Random.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Scenes/ResultScene/ResultScene.h"
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
#include <GameObject/Component/PlayerAnimator.h>

#include "Drawers/Particle/Particle.h"
#include "Scenes/GameScene/GameScene.h"
#include <Scenes/SelectToGame/SelectToGame.h>


static POINTS operator+(const POINTS l, const POINTS r) {
	return POINTS{ static_cast<int16_t>(l.x + r.x), static_cast<int16_t>(l.y + r.y) };
}

static POINTS operator-(const POINTS l, const POINTS r) {
	return POINTS{ static_cast<int16_t>(l.x - r.x), static_cast<int16_t>(l.y - r.y) };
}

static POINTS operator*(const POINTS l, const float r) {
	return POINTS{ static_cast<int16_t>(l.x * r), static_cast<int16_t>(l.y * r) };
}


void GameManager::Init()
{
	GlobalVariables::GetInstance()->LoadFile();

	const SelectToGame *select = SelectToGame::GetInstance();
	LoadGlobalVariant(select->GetSelect());
	SaveGlobalVariant(select->GetSelect());

	Block::StaticLoad();
	DwarfAnimatorComp::StaticLoad();
	EnemyBulletComp::StaticLoad();
	PlayerAnimatorComp::StaticLoad();
	PlayerBulletComp::StaticLoad();
	PlayerComp::StaticLoad();
	AudioManager::GetInstance()->Load("./Resources/Sounds/SE/putBlock.mp3");

	blockGauge_ = std::make_unique<BlockGauge>();
	blockGauge_->Init();

	input_ = Input::GetInstance();

	blockMap_ = std::make_unique<BlockMap>();
	blockMap_->Init();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("./Resources/Cube.obj");

	// 参照を渡す
	LocalBodyComp::pGameManager_ = this;
	LocalBodyComp::pMap_ = GetMap();

	player_ = std::make_unique<GameObject>();
	player_->AddComponent<PlayerComp>();
	player_->AddComponent<PlayerAnimatorComp>();

	// タイマーを用意
	gameTimer_ = std::make_unique<GameTimer>();

	// 初期化する
	gameTimer_->Init();
	gameTimer_->TimerStart(static_cast<float>(vMaxTime_));

	for (int32_t yi = 0; yi < vStartBlockHeight_; yi++) {
		for (int32_t xi = 0; xi < BlockMap::kMapX; xi++) {
			const Vector2 pos = { static_cast<float>(xi), static_cast<float>(yi) };

			Block::BlockType type = static_cast<Block::BlockType>(std::clamp(Lamb::Random(1, *vBlockTypeCount_), 1, static_cast<int32_t>(Block::BlockType::kMax) - 1));

			blockMap_->SetBlocks(pos, Vector2::kIdentity, type);

		}
	}

	player_->GetComponent<LocalBodyComp>()->localPos_ = { 1.f,10.f };


	gameEffectManager_ = std::make_unique<GameEffectManager>();
	gameEffectManager_->Init();

	fallBlockSpawnTimer_.Start(static_cast<float>(vFallBegin_));


	pointTex_ = TextureManager::GetInstance()->LoadTexture("Resources/UI/bonusNumber.png");
}

void GameManager::Update([[maybe_unused]] const float deltaTime)
{

#ifdef _DEBUG

	Debug("GameManager");
	const SelectToGame *select = SelectToGame::GetInstance();
	LoadGlobalVariant(select->GetSelect());

#endif // _DEBUG

	ClearCheck();
	for (auto &item : removeTypes_) {
		item = 0;
	}

	// 演出用のデータの破棄
	gameEffectManager_->Clear();

	GlobalVariables::GetInstance()->Update();

	GetMap()->SetHitMap({});

	const float fixDeltaTime = std::clamp(deltaTime, 0.f, 0.1f);

	blockMap_->Update(fixDeltaTime);
	blockBreakTimer_.Update(fixDeltaTime);

	float localDeltaTime = fixDeltaTime;

	if (blockBreakTimer_.IsActive()) {
		localDeltaTime = fixDeltaTime * std::lerp(0.3f, 0.8f, blockBreakTimer_.GetProgress());
		if (blockBreakTimer_.IsFinish()) {

			gameEffectManager_->blockBreakPos_.first = blockMap_->GetBreakBlockType();
			gameEffectManager_->blockBreakPos_.second = blockMap_->GetBreakBlockMap();

			blockMap_->SetBreakMap({});
			blockMap_->SetBreakBlockMap({});
		}
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

	if (player_) {
		player_->Update(localDeltaTime);
	}

	//spawner_->Update(localDeltaTime);
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

	for (auto &item : itemList_) {
		item->Update(fixDeltaTime);
	}
	AddPoint();

	fallingBlocksPos_.clear();
	bool isLanding = false;

	for (auto &fallingBlock : fallingBlocks_) {
		// 落下しているブロックのコンポーネント
		Lamb::SafePtr fallComp = fallingBlock->GetComponent<FallingBlockComp>();
		// 落下しているブロックの座標
		Lamb::SafePtr blockBody = fallComp->pLocalPos_;

		if (fallingBlock->GetActive() and fallComp->hasDamage_) {
			if (player_) {
				Lamb::SafePtr playerBody = player_->GetComponent<LocalBodyComp>();
				const Vector2 centorDiff = blockBody->localPos_ - playerBody->localPos_;
				const Vector2 sizeSum = (blockBody->size_ + playerBody->size_) / 2.f;
				if (std::abs(centorDiff.x) <= sizeSum.x and std::abs(centorDiff.y) <= sizeSum.y) {
					fallingBlock->OnCollision(player_.get());
					player_->OnCollision(fallingBlock.get());
				}
			}
		}

		// もしブロックがあったら
		if (fallComp->IsLanding()) {
			Audio *audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/putBlock.mp3");
			audio->Start(0.2f, false);
			blockMap_->SetBlocks(blockBody->localPos_, blockBody->size_, fallComp->blockType_.GetBlockType(), fallComp->blockDamage_);
			fallingBlock->SetActive(false);
			if (fallComp->hasDamage_) {
				gameEffectManager_->fallingBlock_.set(static_cast<uint32_t>(blockBody->localPos_.x), false);
			}
			isLanding |= true;
		}

		if (fallingBlock->GetActive()) {
			fallingBlocksPos_.push_back(blockBody->localPos_);
		}
	}

	for (auto &bullet : enemyBulletList_) {
		if (player_) {
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

	// 設置されたら
	if (isLanding) {
		// プレイヤを埋まらない位置に移動する
		PlayerMoveSafeArea();
	}

	damageAreaList_.clear();

	// ボタンを押していない間はゲージを回復する
	if (not Input::GetInstance()->GetKey()->GetKey(DIK_SPACE)) {
		blockGauge_->EnergyRecovery(localDeltaTime);
	}

	//if (player_) {
	//	// プレイヤの体力が0になっていたら終わる
	//	if (player_->GetComponent<HealthComp>()->GetNowHealth() <= 0.f) {
	//		player_.reset();
	//	}
	//}

	gameTimer_->Update(localDeltaTime);

	blockGauge_->Update(localDeltaTime);
	//}
	//gameEffectManager_->fallingBlock_ = spawner_->GetComponent<FallingBlockSpawnerComp>()->GetFutureBlockPos();

	gameEffectManager_->Update(fixDeltaTime);

	if (bonusPointDrawTimer_.IsActive()) {
		bonusPointDrawTimer_.Update(fixDeltaTime);
		bonusTexTransform_.scale = Vector3::kIdentity * SoLib::Lerp(2.5f, 0.5f, SoLib::easeInOutBack(bonusPointDrawTimer_.GetProgress()));
	}

	bonusTexTransform_.CalcMatrix();

	// AABBのデータを転送
	blockMap_->TransferBoxData();
}

void GameManager::Draw([[maybe_unused]] const Camera &camera) const
{
	blockMap_->Draw(camera);
	if (player_) {
		player_->Draw(camera);
	}
	//spawner_->Draw(camera);
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
	for (auto &item : itemList_) {
		item->Draw(camera.GetViewOthographics());
	}
	blockGauge_->Draw(camera);

	gameEffectManager_->Draw(camera);

	if (bonusPointDrawTimer_.IsActive()) {
		DrawerManager::GetInstance()->GetTexture2D()->Draw(bonusTexTransform_.matWorld_, Mat4x4::MakeAffin({ 0.25f,1.f,1.f }, Vector3{}, { 0.25f * (itemSpawnCount_ - 1),0,0 }), camera.GetViewOthographics(), pointTex_, 0xFFFFFFFF, BlendType::kNormal);
	}
}

void GameManager::LoadGlobalVariant([[maybe_unused]] const uint32_t stageIndex)
{
	const auto *const gVariable = GlobalVariables::GetInstance();
	{
		const auto *const group = gVariable->GetGroup("Stage" + std::to_string(stageIndex));
		if (group) {
			LoadValue(*group, *GetInstance(), vGameManagerItems_);
		}
	}
	{
		const auto *const group = gVariable->GetGroup("BlockMap");
		if (group) {
			LoadValue(*group, vBlockMapItems_);
		}
	}
	{
		const auto *const group = gVariable->GetGroup("PlayerComp");
		if (group) {
			LoadValue(*group, PlayerComp::vPlayerItems_);
		}
	}
	{
		const auto *const group = gVariable->GetGroup("DwarfComp");
		if (group) {
			LoadValue(*group, DwarfComp::vDwarfItems_);
		}
	}
	{
		const auto *const group = gVariable->GetGroup("ItemStatus");
		if (group) {
			LoadValue(*group, vItemStatus_);
		}
	}
	{
		const auto *const group = gVariable->GetGroup("FallBlockStatus");
		if (group) {
			LoadValue(*group, FallingBlockComp::vFallBlockStatus_);
		}
	}

}

void GameManager::SaveGlobalVariant([[maybe_unused]] const uint32_t stageIndex) const
{

#ifdef _DEBUG

	auto *const gVariable = GlobalVariables::GetInstance();
	{
		auto *const group = gVariable->AddGroup("Stage" + std::to_string(stageIndex));
		if (group) {
			SaveValue(*group, *GetInstance(), vGameManagerItems_);
		}
	}
	{
		auto *const group = gVariable->AddGroup("BlockMap");
		if (group) {
			SaveValue(*group, vBlockMapItems_);

		}
	}
	{
		auto *const group = gVariable->AddGroup("PlayerComp");
		if (group) {
			SaveValue(*group, PlayerComp::vPlayerItems_);
		}
	}
	{
		auto *const group = gVariable->AddGroup("DwarfComp");
		if (group) {
			SaveValue(*group, DwarfComp::vDwarfItems_);
		}
	}
	{
		auto *const group = gVariable->AddGroup("ItemStatus");
		if (group) {
			SaveValue(*group, vItemStatus_);
		}
	}
	{
		auto *const group = gVariable->AddGroup("FallBlockStatus");
		if (group) {
			SaveValue(*group, FallingBlockComp::vFallBlockStatus_);
		}
	}
#endif // _DEBUG

}

bool GameManager::Debug([[maybe_unused]] const char *const str)
{

	bool isChange = false;

#ifdef _DEBUG

	ImGui::Begin(str);

	//blockMap_->Debug("BlockMap");
	SoLib::ImGuiWidget(&vFallSpan_);
	SoLib::ImGuiText("アイテム数", std::to_string(itemCount_) + '/' + std::to_string(vClearItemCount_));
	SoLib::ImGuiText("残り時間", std::to_string(gameTimer_->GetDeltaTimer().GetNowFlame()) + '/' + std::to_string(gameTimer_->GetDeltaTimer().GetGoalFlame()));

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

GameObject *GameManager::AddFallingBlock(Vector2 centerPos, Vector2 size, Block::BlockType blockType, Vector2 velocity, Vector2 gravity, bool hasDamage, uint32_t blockDamage)
{
	std::unique_ptr<GameObject> addBlock = std::make_unique<GameObject>();

	// 落下するブロックのコンポーネント
	Lamb::SafePtr fallingComp = addBlock->AddComponent<FallingBlockComp>();
	Lamb::SafePtr localBodyComp = addBlock->AddComponent<LocalBodyComp>();
	Lamb::SafePtr localHitMap = addBlock->AddComponent<LocalMapHitComp>();

	localHitMap->isHitFallBlock_ = false;

	localBodyComp->localPos_ = centerPos;
	localBodyComp->size_ = size;

	fallingComp->blockType_ = blockType;
	fallingComp->velocity_ = velocity;
	fallingComp->gravity_ = gravity;
	fallingComp->hasDamage_ = hasDamage;

	fallingComp->blockDamage_ = blockDamage;

	addBlock->GetComponent<LocalRigidbody>();

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
		//gameEffectManager_->blockBreakPos_.push_back({ centerPos, size });

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
	localBody->localPos_ = { std::round(centerPos.x), centerPos.y }; // 座標の指定
	localBody->drawScale_ = 1.f;

	dwarf->AddComponent<DwarfAnimatorComp>();
	dwarf->AddComponent<DwarfShakeComp>();

	dwarfComp->updateFunc_.push_back(&DwarfComp::FireBullet);

	// 末尾に追加
	darkDwarfList_.push_back(std::move(dwarf));
	// 参照を返す
	return darkDwarfList_.back().get();
}

GameObject *GameManager::AddDwarf(std::unique_ptr<GameObject> dwarf)
{
	if (dwarf->GetComponent<DwarfComp>()->GetIsDarkDwarf()) {
		darkDwarfList_.push_back(std::move(dwarf));
		return darkDwarfList_.back().get();
	}
	else {
		dwarfList_.push_back(std::move(dwarf));
		return dwarfList_.back().get();
	}
}


BlockMap::BlockBitMap &&GameManager::BreakChainBlocks(POINTS localPos)
{
	auto block = blockMap_->GetBlockType(localPos);

	const auto &dwarfPosSet = GetDwarfPos();

	auto &&chainBlockMap = blockMap_->FindChainBlocks(localPos, blockMap_->GetBlockType(localPos), dwarfPosSet);

	std::pair<Vector2, Vector2> minMax{ {-1,-1}, {-1,-1} };

	for (int16_t yi = 0; yi < chainBlockMap.size(); yi++) {

		for (int16_t xi = 0; xi < chainBlockMap[yi].size(); xi++) {
			if (chainBlockMap[yi][xi]) {
				blockBreakTimer_.Start(vBreakStopTime_);
				bonusPointDrawTimer_.Start(1.f);
				if (minMax.first.x == -1) {
					minMax.first.x = static_cast<float>(xi);
				}
				if (minMax.first.y == -1) {
					minMax.first.y = static_cast<float>(yi);
				}

				minMax.second.x = static_cast<float>(xi);
				minMax.second.y = static_cast<float>(yi);
			}
		}
	}

	Vector2 center = SoLib::Lerp(minMax.first, minMax.second, 0.5f);

	POINTS targetPos{};

	// 壊れたブロックだけのデータ
	BlockMap::BlockBitMap breakBlock = chainBlockMap;
	// 何個壊したか
	size_t breakCount = 0;

	std::vector<GameObject *> deadDwarfList; deadDwarfList.reserve(dwarfList_.size());
	std::vector<GameObject *> deadDarkDwarfList; deadDarkDwarfList.reserve(darkDwarfList_.size());

	for (const POINTS pos : dwarfPosSet) {
		if (not BlockMap::IsOutSide(pos)) {
			// もし、ブロックが存在しなかった場合はそこを折る
			if (blockMap_->GetBlockType(pos) != block) {
				breakBlock[pos.y][pos.x] = false;
			}
		}
	}

	// ブロックの数を保存
	for (const decltype(breakBlock)::value_type line : breakBlock) {
		breakCount += line.count();
	}

	// 通常小人の死亡検知
	for (auto &dwarf : dwarfList_) {
		auto localBody = dwarf->GetComponent<LocalBodyComp>();
		if (localBody) {
			POINTS mapIndex = localBody->GetMapPos();
			if (not BlockMap::IsOutSide(mapIndex)) {
				// そこが破壊対象なら値を追加する
				if (chainBlockMap[mapIndex.y][mapIndex.x]) {
					deadDwarfList.push_back(dwarf.get());
					breakCount++;
				}
			}
		}
	}

	// 闇小人の死亡検知
	for (auto &dwarf : darkDwarfList_) {
		auto localBody = dwarf->GetComponent<LocalBodyComp>();
		if (localBody) {
			POINTS mapIndex = localBody->GetMapPos();
			if (not BlockMap::IsOutSide(mapIndex)) {
				// そこが破壊対象なら値を追加する
				if (chainBlockMap[mapIndex.y][mapIndex.x]) {
					deadDarkDwarfList.push_back(dwarf.get());
					breakCount++;
				}
			}
		}
	}

	if (breakCount <= 3) {
		itemSpawnCount_ = 1;
	}
	else if (breakCount <= 6) {
		itemSpawnCount_ = 2;
	}
	else if (breakCount <= 9) {
		itemSpawnCount_ = 3;
	}
	else {
		itemSpawnCount_ = 4;
	}


	//Block::BlockType breakBlockType = Block::BlockType::kNone;

	for (targetPos.y = 0; targetPos.y < BlockMap::kMapY; targetPos.y++) {
		const auto &breakLine = breakBlock[targetPos.y];
		for (targetPos.x = 0; targetPos.x < BlockMap::kMapX; targetPos.x++) {
			if (breakLine[targetPos.x]) {
				const auto blockType = blockMap_->GetBlockType(targetPos);
				blockMap_->BreakBlock(targetPos);

				AddItem(BlockMap::GetGlobalPos(targetPos), blockType, itemSpawnCount_);
				//breakBlockType = blockType;

			}
		}
	}

	for (auto *const dwarf : deadDwarfList) {

		AddItem(dwarf->GetComponent<LocalBodyComp>()->GetGlobalPos(), Block::BlockType::kBlue, itemSpawnCount_);
		dwarf->SetActive(false);

	}

	for (auto *const dwarf : deadDarkDwarfList) {
		AddItem(dwarf->GetComponent<LocalBodyComp>()->GetGlobalPos(), Block::BlockType::kRed, itemSpawnCount_);
		dwarf->SetActive(false);
	}

	// どれか一体でも死んでたら鳴らす
	if (not deadDwarfList.empty() or not deadDarkDwarfList.empty()) {
		Audio *audio = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/slimeDeath.mp3");
		audio->Start(0.2f, false);
	}



	blockMap_->SetBreakBlockType(block);
	blockMap_->SetBreakBlockMap(breakBlock);

	blockMap_->SetBreakMap(chainBlockMap);

	bonusTexTransform_.translate = Vector3{ BlockMap::GetGlobalPos(center), -15.f };

	return std::move(chainBlockMap);
}
BlockMap::BlockBitMap &&GameManager::HitChainBlocks(POINTS localPos) {

	if (auto block = Block{ blockMap_->GetBlockType(localPos) }; block) {
		blockMap_->SetDamageColor(block.GetColor());
	}

	auto &&chainBlockMap = blockMap_->FindChainBlocks(localPos, blockMap_->GetBlockType(localPos), GetDwarfPos());

	blockMap_->SetHitMap(chainBlockMap);

	return std::move(chainBlockMap);
}
std::list<GameManager::DwarfPick> GameManager::PickUpObject(const POINTS localPos)
{
	std::list<DwarfPick> result;

	Lamb::SafePtr localBody = player_->GetComponent<LocalBodyComp>();

	{
		for (auto dwarfItr = dwarfList_.begin(); dwarfItr != dwarfList_.end();) {
			Lamb::SafePtr dwarfObject = dwarfItr->get();
			Lamb::SafePtr body = dwarfObject->GetComponent<LocalBodyComp>();
			const POINTS bodyPos = body->GetMapPos();

			// もし隣であったら取る
	//		if (std::abs(bodyPos.x - localPos.x) + std::abs(bodyPos.y - localPos.y) <= 1) {
			if (bodyPos == localPos) {

				result.push_back({ body->localPos_ - Vector2{static_cast<float>(localPos.x), static_cast<float>(localPos.y)}, std::move(*dwarfItr) });

				dwarfItr = dwarfList_.erase(dwarfItr); // オブジェクトを破棄してイテレータを変更
				continue;
			}

			// 何もなかったら次へ
			++dwarfItr;
		}
		for (auto dwarfItr = darkDwarfList_.begin(); dwarfItr != darkDwarfList_.end();) {
			Lamb::SafePtr dwarfObject = dwarfItr->get();
			Lamb::SafePtr body = dwarfObject->GetComponent<LocalBodyComp>();
			const POINTS bodyPos = body->GetMapPos();

			// もし隣であったら取る
	//		if (std::abs(bodyPos.x - localPos.x) + std::abs(bodyPos.y - localPos.y) <= 1) {
			if (bodyPos == localPos) {

				result.push_back({ body->localPos_ - Vector2{static_cast<float>(localPos.x), static_cast<float>(localPos.y)}, std::move(*dwarfItr) });

				dwarfItr = darkDwarfList_.erase(dwarfItr); // オブジェクトを破棄してイテレータを変更
				continue;
			}

			// 何もなかったら次へ
			++dwarfItr;
		}
	}


	return result;
}

void GameManager::RandomDwarfSpawn()
{
	if (not dwarfSpawnTimer_.IsActive()) {
		dwarfSpawnTimer_.Start(vSpawnSpan_);
		int32_t spawnPos = Lamb::Random(0, BlockMap::kMapX - 1);
		AddDwarf(Vector2{ static_cast<float>(spawnPos), 0 });
	}
}
void GameManager::RandomFallBlockSpawn()
{
	if (not fallBlockSpawnTimer_.IsActive()) {
		fallBlockSpawnTimer_.Start(vFallSpan_);
		/*	std::vector<uint8_t> vec;
			vec.reserve(BlockMap::kMapX - gameEffectManager_->fallingBlock_.count());
			for (uint8_t i = 0; i < BlockMap::kMapX; i++) {
				if (not gameEffectManager_->fallingBlock_.test(i)) {
					vec.push_back(i);
				}
			}
		*/

		// 高い場所をもとに割合を出す
		std::vector<uint8_t> randVec;

		std::array<int16_t, BlockMap::kMapX> mapHeight{};
		int16_t heighest = -1;

		for (int16_t yi = 0u; yi < BlockMap::kMapY; yi++) {
			for (int16_t xi = 0u; xi < BlockMap::kMapX; xi++) {
				Block::BlockType block = blockMap_->GetBlockType(POINTS{ xi,yi });
				// ブロックが存在する場合
				if (block != Block::BlockType::kNone) {
					mapHeight[xi] = yi;
					heighest = yi;
				}

			}
			if (heighest != yi) {
				break;
			}
		}

		std::vector<uint8_t> targets{};
		for (int32_t i = 0; i < mapHeight.size(); i++) {
			// もし生成してるなら飛ばす
			if (gameEffectManager_->fallingBlock_.test(i)) {
				continue;
			}
			for (int32_t c = 0; c < (heighest == -1 ? 1 : (heighest - mapHeight[i]) * vFallPosCalc_ + 1); c++) {
				targets.push_back(static_cast<uint8_t>(i));
			}
		}



		const int32_t spawnPos = targets[Lamb::Random(0, static_cast<int32_t>(targets.size() - 1))];
		const uint32_t blockType = std::clamp(Lamb::Random(1, *vBlockTypeCount_), 1, static_cast<int32_t>(Block::BlockType::kMax) - 1);

		gameEffectManager_->fallingBlock_.set(spawnPos);

		AddFallingBlock(Vector2{ static_cast<float>(spawnPos), static_cast<float>(BlockMap::kMapY) }, Vector2::kIdentity, static_cast<Block::BlockType>(blockType), Vector2::kYIdentity * -5, Vector2::kZero);
	}

	//for (uint32_t i = 0; i < spawnLeftTime_.size(); i++) {
	//	auto &&[spawnTime, type] = spawnLeftTime_[i];
	//	if (spawnTime > 0) {
	//		spawnTime = std::clamp(spawnTime - deltaTime, 0.f, *vBreakStopTime_);
	//		if (spawnTime <= 0) {
	//			AddFallingBlock(Vector2{ static_cast<float>(i), static_cast<float>(BlockMap::kMapY) }, Vector2::kIdentity, type, Vector2::kYIdentity * -5, Vector2::kZero);
	//		}
	//	}
	//}

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

void GameManager::AddItem([[maybe_unused]] const Vector2 globalPos, const Block::BlockType blockType, const uint32_t count)
{
	// もしブロックが無効であったら飛ばす
	if (blockType == Block::BlockType::kNone) { return; }

	// ブロックを追加する処理｡仮なので､float型の時間だけを格納している｡
	for (uint32_t i = 0; i < count; i++) {
		Vector2 endPos = { -8.8f,11.5f };

		std::unique_ptr<BlockItem> item = std::make_unique<BlockItem>(globalPos, endPos, blockType, i * vItemSpawnSpan_);
		itemList_.emplace_back(std::move(item));
	}
	// ↑ アイテムのクラスができたら､この処理を置き換える
}

void GameManager::AddPoint()
{
	// for文を回して､1つずつ時間が終わっていないか確認する｡
	for (auto item = itemList_.cbegin(); item != itemList_.end();) {

		// 時間が0以下になってたら消す
		if ((*item)->GetIsEnd()) {

			// 破壊時の処理
			itemCount_++;

			// アイテムの属性で数値を増やす
			uint32_t index = static_cast<uint32_t>((*item)->GetItemType());
			index = std::clamp(index, 1u, static_cast<uint32_t>(Block::BlockType::kMax) - 1) - 1;
			itemTypeCount_[index]++;

			UIEditor::GetInstance()->BeginScaleMove(UIEditor::scaleMoveSpeed);


			item = itemList_.erase(item); // オブジェクトを破棄してイテレータを変更
			continue;
		}

		// 何もなかったら次へ
		++item;
	}
}

struct CountIndex {
	int32_t count_;
	int32_t index_;
};

static bool comp(const CountIndex a, const CountIndex b) { return  a.count_ > b.count_; }


void GameManager::RemovePoint(const int32_t count)
{
	int32_t total = std::min(count, itemCount_);
	itemCount_ -= total;
	//itemCount_ = std::clamp(itemCount_, 0, *vClearItemCount_);

	std::array<CountIndex, 5u> indexedArr{};
	for (int32_t i = 0; i < 5u; ++i) {
		indexedArr[i] = { itemTypeCount_[i], i };
	}

	// ソート（値が大きい順に並べる）
	std::sort(indexedArr.rbegin(), indexedArr.rend(), comp);


	for (uint32_t i = 0; i < itemTypeCount_.size(); i++) {
		auto &item = itemTypeCount_[indexedArr[i].index_];
		auto &removeTypes = removeTypes_[indexedArr[i].index_];

		const int32_t removeCount = total / static_cast<int32_t>(itemTypeCount_.size() - i);
		const int32_t result = item - removeCount;

		// もし減少のほうが多かったらちょっと色々する
		if (result < 0) {
			total -= item;
			removeTypes = item;
			item = 0;
		}
		else {
			total -= removeCount;
			item -= removeCount;
			removeTypes = removeCount;
		}
	}
}

void GameManager::InputAction()
{
	{
		// スポナーのコンポーネント
		//Lamb::SafePtr spawnerComp = spawner_->GetComponent<FallingBlockSpawnerComp>();
		//Lamb::SafePtr key = input_->GetKey();

		// DIK_DOWN を押したときに実行
		//if (key->Pushed(DIK_DOWN)) {
		//	spawnerComp->SetStartPos(); // 落下開始地点を設定
		//}
		//if (key->Released(DIK_DOWN)) {
		//	spawnerComp->SpawnFallingBlock(); // 落下ブロックを追加
		//}

		//if (key->Pushed(DIK_UP)) {
		//	spawnerComp->fallBlockType_ = static_cast<Block::BlockType>(static_cast<uint32_t>(spawnerComp->fallBlockType_.GetBlockType()) + 1);
		//	if (not spawnerComp->fallBlockType_) {
		//		spawnerComp->fallBlockType_ = Block::BlockType::kRed;
		//	}
		//}

		//// スポナーに付与する移動方向
		//int32_t inputSpawner{};

		//// 横方向の移動
		//inputSpawner -= key->Pushed(DIK_LEFT);
		//inputSpawner += key->Pushed(DIK_RIGHT);

		//spawnerComp->MoveInput(inputSpawner);
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
				AddFallingBlock(localPos, Vector2::kIdentity, block.GetBlockType(), Vector2::kYIdentity * -5, Vector2::kZero, false, block.GetDamage());
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

void GameManager::ClearCheck()
{
	if (not player_) {
		ResultScene::SetIsGameClear(false);
		pGameScene_->ChangeToResult();
	}
	if (vClearItemCount_ < itemCount_) {
		ResultScene::SetIsGameClear(true);
		pGameScene_->ChangeToResult();
	}
	if (gameTimer_->IsFinish()) {
		ResultScene::SetIsGameClear(false);
		pGameScene_->ChangeToResult();
	}

}

void GameManager::PlayerMoveSafeArea()
{
	const Vector2 playerPos = player_->GetComponent<LocalBodyComp>()->localPos_;
	const POINTS intPos = { static_cast<int16_t>(playerPos.x + 0.5f), static_cast<int16_t>(playerPos.y + 0.5f) };


	if (not BlockMap::IsOutSide(intPos)) {
		// もし､空なら終わり
		if (blockMap_->GetBlockType(intPos) == Block::BlockType::kNone) {
			return;
		}
		int16_t begin = 1;
		// 左右が埋まっていたならそこに置く
		if (blockMap_->GetBlockType(intPos + POINTS{ 1, 0 }) != Block::BlockType::kNone and blockMap_->GetBlockType(intPos - POINTS{ 1, 0 }) != Block::BlockType::kNone) {
			begin = 0;
		}

		bool left = true, right = true;
		for (int16_t xi = begin; xi < BlockMap::kMapX and (left or right); xi++) {
			if (left) {
				if (BlockMap::IsOutSide(intPos + POINTS{ static_cast<int16_t>(-xi), 0 })) {
					left = false;
				}

			}
			if (right) {
				if (BlockMap::IsOutSide(intPos + POINTS{ xi, 0 })) {
					right = false;
				}

			}
			for (int16_t yi = 0; yi < BlockMap::kMapY - intPos.y; yi++) {
				const POINTS offsetX = { xi, 0 };
				const POINTS targetPos = intPos + POINTS{ 0, yi };

				const bool leftVoid = left && blockMap_->GetBlockType(targetPos - offsetX) == Block::BlockType::kNone;
				const bool rightVoid = right && blockMap_->GetBlockType(targetPos + offsetX) == Block::BlockType::kNone;

				if (leftVoid) {
					player_->GetComponent<LocalBodyComp>()->localPos_ = { static_cast<float>(targetPos.x - xi),static_cast<float>(targetPos.y) };
					return;
				}
				if (rightVoid) {
					player_->GetComponent<LocalBodyComp>()->localPos_ = { static_cast<float>(targetPos.x + xi),static_cast<float>(targetPos.y) };
					return;
				}
			}
		}
	}

}
