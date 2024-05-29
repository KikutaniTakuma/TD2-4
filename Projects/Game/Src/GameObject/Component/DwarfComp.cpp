#include "DwarfComp.h"
#include "ModelComp.h"
#include "SpriteAnimatorComp.h"
#include "SpriteComp.h"
#include <Drawers/DrawerManager.h>
#include <Utils/Random/Random.h>

void DwarfComp::Init()
{
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();
	pPickUpComp_ = object_.AddComponent<PickUpComp>();

	pLocalBodyComp_->size_ = Vector2::kIdentity * 0.5f;

	/*Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	Lamb::SafePtr bodyModel = modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel(GameManager::kDwarfModelName));
	bodyModel->color_ = 0xFF0000FF;*/

	/*{
			Lamb::SafePtr spriteComp = object_.AddComponent<SpriteComp>();
			spriteComp->SetTexture("./Resources/uvChecker.png");

			spriteComp->CalcTexUv();
	}
	{
			Lamb::SafePtr spriteAnimComp = object_.AddComponent<SpriteAnimatorComp>();
	}*/

	killSE_ = AudioManager::GetInstance()->Load("./Resources/Sounds/SE/dwarfKill.mp3");
}

void DwarfComp::Start()
{
}

void DwarfComp::Update()
{

	// もし埋まってたら上に登る
	ClimbUp();
	// 登ってない場合
	if (isClimbing_ == false) {
		// 落ちる処理を行う
		bool isFall = FallDown();

		// もし着地しているなら
		if (not isFall) {
			// 移動先が定まっている場合
			if (targetPos_ != -Vector2::kIdentity) {

				// 移動を行う
				if (MoveBlockPos(targetPos_)) // ゴールに到達した場合
				{
					// ターゲットを無効にする
					targetPos_ = -Vector2::kIdentity;
				}
			}
		}

		for (const auto &func : updateFunc_) {
			(this->*func)();
		}

		// 登っているブロックごとの処理
		//ChangeMovementTarget();
	}

	if (pPickUpComp_->GetBlockWeight() > 6) {
		object_.SetActive(false);
		killSE_->Start(0.2f, false);
	}

	// データの転送
	pLocalBodyComp_->TransfarData();
	transform_.translate.z = -3.f - 0.1f * pPickUpComp_->GetBlockWeight();
}

void DwarfComp::ClimbUp()
{
	const Vector2 localPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * (0.5f + 0.25f * facing_);
	// 現在のブロック
	auto blockType = LocalBodyComp::pMap_->GetBlockType(localPos);

	bool climbFlag = false;
	// ブロックの中にいる場合登る
	climbFlag |= static_cast<bool>(blockType);

	// 登るフラグが立っていたら登る
	if (climbFlag) {
		pLocalBodyComp_->localPos_.y += GetDeltaTime() * vClimbSpeed_;
		isClimbing_ = true;
	}
	// 折れていたら登っていない
	else {
		isClimbing_ = false;
	}
}

bool DwarfComp::FallDown()
{

	const auto &blocks = GameManager::GetInstance()->GetFallingBlocksPos();
	const float downPower = 2.f * GetDeltaTime();
	float afterPosY = pLocalBodyComp_->localPos_.y - downPower;
	// 落下先の座標
	const Vector2 downSide = Vector2{ pLocalBodyComp_->localPos_.x + 0.5f, afterPosY };
	if (afterPosY > 0) {
		// 落下先がブロックでない場合 なおかつ前がブロックでない場合
		if (LocalBodyComp::pMap_->GetBlockType(downSide) == Block::BlockType::kNone and LocalBodyComp::pMap_->GetBlockType(downSide + Vector2::kXIdentity * (0.25f * facing_)) == Block::BlockType::kNone) {

			for (const Vector2 pos : blocks) {
				//Vector2 targetPos;
				if (std::abs(downSide.x - pos.x) <= 0.5f and std::abs(downSide.y - pos.y) <= 1) {
					pLocalBodyComp_->localPos_.y = pos.y + 1;
					return false;
				}
			}

			// 下げる
			pLocalBodyComp_->localPos_.y -= downPower;
			return true;

		}
		// 足場があった場合
		else {
			// 足場の上に強制的に置く
			pLocalBodyComp_->localPos_.y = std::floor(downSide.y) + 1.f;
		}
	}
	else {
		pLocalBodyComp_->localPos_.y = 0.f;
	}

	return false;
}

void DwarfComp::ChangeMovementTarget()
{
	// 現在の中心座標
	//const Vector2 localPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * 0.5f;

	// ブロックを持っていたら帰る
	//CarryBlockForHouse();

	// 自分の位置が拠点の場合
	//LocateOnHouse();

	// 自由に動き回る
	FreeTargetMove();
}

void DwarfComp::FireBullet()
{
	timer_.Update(GetDeltaTime());
	Lamb::SafePtr player = GameManager::GetInstance()->GetPlayer();
	if (player) {

		Lamb::SafePtr plBody = player->GetComponent<LocalBodyComp>();
		facing_ = static_cast<int32_t>(SoLib::Math::Sign(plBody->localPos_.x - pLocalBodyComp_->localPos_.x));
	}
	if (not timer_.IsActive()) {


		timer_.Start(vBulletFireSpan_);

		Lamb::SafePtr pGManager = GameManager::GetInstance();

		if (facing_) {
			const Vector2 facingVec = Vector2::kXIdentity * static_cast<float>(facing_);

			pGManager->AddEnemyBullet(pLocalBodyComp_->localPos_ + facingVec, Vector2::kXIdentity * (facing_ * vBulletSpeed_));
		}
	}
}

bool DwarfComp::GetIsDarkDwarf() const
{
	return std::count(updateFunc_.begin(), updateFunc_.end(), &DwarfComp::FireBullet);
}

// ブロックを持っていたら帰る
void DwarfComp::CarryBlockForHouse()
{
	// 持っているブロックの数
	// int32_t blockCount = pPickUpComp_->GetBlockWeight();

	//// 拠点のどこにいるか
	// int32_t onLocatePos = GetLocatePosEnemyHouse();

	// もし拠点の外で､ブロックを持っていたら
	// if (onLocatePos == -1 && blockCount > 0)
	//{
	//	// 最も近い拠点のデータ
	//	Lamb::SafePtr nearestHouse = LocalBodyComp::pMap_->GetNearestHouse(static_cast<int32_t>(pLocalBodyComp_->localPos_.x + 0.5f));

	//	// 拠点のデータがあれば
	//	if (nearestHouse != nullptr)
	//	{
	//		// 最も近い拠点の反対側
	//		int32_t targetPos = SoLib::Math::Sign(nearestHouse->xPos_ - static_cast<int32_t>(pLocalBodyComp_->localPos_.x + 0.5f));

	//		// 最も近い拠点に帰る
	//		targetPos_ = Vector2{ static_cast<float> (nearestHouse->xPos_ + targetPos), 0 };
	//	}
	//}
}

int32_t DwarfComp::GetLocatePosEnemyHouse() const
{
	// 拠点の横の長さ
	// constexpr int32_t houseLength = 3;

	//// 最も近い拠点のデータ
	// Lamb::SafePtr nearestHouse = LocalBodyComp::pMap_->GetNearestHouse(static_cast<int32_t>(pLocalBodyComp_->localPos_.x + 0.5f));

	//// データが存在しなかったら無効の値を入れる
	// if (nearestHouse == nullptr) { return -1; }

	//// 拠点の中心座標
	// int32_t centerPos = nearestHouse->xPos_;
	//// 拠点の左端
	// int32_t leftSidePos = centerPos - 1;

	//// 左端からの距離
	// int32_t distance = static_cast<int32_t>(pLocalBodyComp_->localPos_.x + 0.5f) - leftSidePos;

	//// 自分の下の左2マス以内に拠点がある場合
	// if (distance >= 0 && distance < houseLength)
	//{
	//	// 距離を返す
	//	return distance;
	// }

	// 存在しないという値を入れる
	return -1;
}
void DwarfComp::LocateOnHouse()
{
	// Lamb::SafePtr map = LocalBodyComp::pMap_;

	//// 現在の中心座標
	// const Vector2 localPos = pLocalBodyComp_->localPos_ + Vector2::kXIdentity * 0.5f;
	//// 自分の位置が虚空の場合
	// if (map->GetBoxType(localPos) == Block::BlockType::kNone)
	//{
	//	// 拠点の左端からの距離
	//	int onLocatePos = GetLocatePosEnemyHouse();

	//	// 拠点の上にいる場合
	//	if (onLocatePos != -1)
	//	{
	//		// もしブロックの数が1つ以上あれば
	//		if (not pPickUpComp_->GetPickUpBlockList().empty())
	//		{
	//			// 持っているブロックの最初のデータを取得
	//			auto firstPickupData = pPickUpComp_->GetPickUpBlockList().front();

	//			// ブロックの長さ
	//			const Vector2 &blockSize = firstPickupData.size_;

	//			const Vector2 halfSize = (blockSize - Vector2::kIdentity) / 2.f;

	//			// もしブロックの長さが拠点の長さより短かったら
	//			if (blockSize.x <= 3)
	//			{

	//				// 設置する中心座標
	//				Vector2 blockCenterPos = Vector2::kYIdentity * (std::floor(localPos.y) + (blockSize.y / 2 - 0.5f));

	//				float blockSizeDiff = 0;

	//				switch (static_cast<int32_t>(blockSize.x))
	//				{
	//				case 2:
	//					if (facing_ == 1)
	//					{
	//						blockSizeDiff = onLocatePos - 0.5f;
	//					}
	//					else
	//					{
	//						blockSizeDiff = onLocatePos - 1.5f;
	//					}

	//					break;
	//				case 3:

	//					blockSizeDiff = onLocatePos - 1.f;

	//					break;
	//				}

	//				// 中心座標を設定
	//				blockCenterPos.x = std::floor(localPos.x) - blockSizeDiff;

	//				// 設置できるか調べる
	//				bool isSetable = true;
	//				// 自分の位置から､facing_の方向に加算して､その方向がすべて虚空であれば設置する
	//				for (float y = 0; y < blockSize.y; y++)
	//				{
	//					for (float x = 0; x < blockSize.x; x++)
	//					{
	//						if (map->GetBoxType(blockCenterPos + Vector2::kXIdentity * x + Vector2::kYIdentity * y - halfSize) != Block::BlockType::kNone)
	//						{
	//							isSetable = false;
	//							break;
	//						}
	//					}
	//				}

	//				// 設置可能であれば設置する
	//				if (isSetable)
	//				{
	//					// ブロックを配置
	//					//map->SetBlocks(blockCenterPos, blockSize, Map::BoxType::kEnemyBlock);

	//					// 先頭要素を削除
	//					pPickUpComp_->PopFront();
	//				}
	//			}

	//		}

	//	}

	//}
}

void DwarfComp::FreeTargetMove()
{
	// ブロックを持っていたら終わる
	if (pPickUpComp_->GetBlockWeight() > 0) {
		return;
	}
	if (movementFacing_ == 0) {
		movementFacing_ = Lamb::Random(0, 1) ? 1 : -1;
	}
	else {

		// ローカルの座標
		float localX = pLocalBodyComp_->localPos_.x;

		if (targetPos_ == -Vector2::kIdentity) {
			movementFacing_ = 1;
		}

		// 右端にいる場合
		if ((BlockMap::kMapX - 1.5f) <= localX) {
			// 左端に行くように指定
			movementFacing_ = -1;
		}
		// 左端にいる場合
		else if (0.5f >= localX) {
			// 右端に行くように指定
			movementFacing_ = 1;
		}
	}
	// 向いている方向の端に移動先を設定する
	if (movementFacing_ == 1) {
		// 右下に移動
		targetPos_ = Vector2::kXIdentity * ((BlockMap::kMapX - 1));
	}
	else if (movementFacing_ == -1) {
		// 左下に移動
		targetPos_ = Vector2::kZero;
	}
}
