#include "LocalMapHitComp.h"
#include "Game/GameManager/GameManager.h"

void LocalMapHitComp::Init()
{
	// マップの参照を渡す
	pBlockMap_ = GameManager::GetInstance()->GetMap();

	// 体の判定を取得
	pRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pLocalBodyComp_ = object_.AddComponent<LocalBodyComp>();

}

void LocalMapHitComp::Update()
{
	enum X {
		kLeft,
		kRight,
	};

	enum Y {
		kDown,
		kUp,
	};

	Vector2 hitNormal = {};
	Vector2 velocity = pRigidbody_->GetVelocity();

	const Vector2 centor = pLocalBodyComp_->localPos_ + Vector2::kIdentity * 0.5f;

	const Vector2 centorDown = centor + Vector2{ 0, pLocalBodyComp_->size_.y * -0.5f };
	const Vector2 centorTop = centor + Vector2{ 0, pLocalBodyComp_->size_.y * +0.5f };
	const Vector2 centorLeft = centor + Vector2{ pLocalBodyComp_->size_.x * -0.5f, 0 };
	const Vector2 centorRight = centor + Vector2{ pLocalBodyComp_->size_.x * +0.5f, 0 };

	std::array<float, 2u> xPos{ centor.x + pLocalBodyComp_->size_.x * -0.5f, centor.x + pLocalBodyComp_->size_.x * +0.5f };
	std::array<float, 2u> yPos{ centor.y + pLocalBodyComp_->size_.y * -0.5f, centor.y + pLocalBodyComp_->size_.y * +0.5f };

	//const Vector2 leftDown = centor - pLocalBodyComp_->size_ * 0.5f;
	//const Vector2 rightTop = centor + pLocalBodyComp_->size_ * 0.5f;

	//[[maybe_unused]] const Vector2 leftTop = { leftDown.x, rightTop.y };
	//[[maybe_unused]] const Vector2 rightDown = { rightTop.x, leftDown.y };

	// 左が画面外だったら
	if (centorLeft.x < 0) {
		pLocalBodyComp_->localPos_.x = 0 - 0.5f + pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
		hitNormal.x = 1.f;
	}
	// 右が画面外だったら
	else if (centorRight.x >= static_cast<float>(BlockMap::kMapX)) {
		pLocalBodyComp_->localPos_.x = BlockMap::kMapX - 0.5f - pLocalBodyComp_->size_.x * 0.5f;
		velocity.x = 0;
		hitNormal.x = -1.f;
	}
	// 下が画面外だったら
	if (centorDown.y < 0) {
		pLocalBodyComp_->localPos_.y = 0 - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
		velocity.y = 0;
		hitNormal.y = 1.f;
	}


	{	// 縦方向
		{	// 下方向
			if (pBlockMap_->GetBlockType(centorDown) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::ceil(centorDown.y) - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = 1.f;
			}
			else if (Vector2 targetPos = { xPos[kLeft], yPos[kDown] }; pBlockMap_->GetBlockType(targetPos) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::ceil(centorDown.y) - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = 1.f;
			}
			else if ( targetPos = { xPos[kRight], yPos[kDown] }; pBlockMap_->GetBlockType(targetPos) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::ceil(centorDown.y) - 0.5f + pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = 1.f;
			}

		}
		{	// 上方向
			if (pBlockMap_->GetBlockType(centorTop) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::floor(centorTop.y) - 0.5f - pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = -1.f;
			}
			else if (Vector2 targetPos = { xPos[kLeft], yPos[kUp] }; pBlockMap_->GetBlockType(targetPos) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::floor(centorTop.y) - 0.5f - pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = -1.f;
			}
			else if (targetPos = { xPos[kRight], yPos[kUp] }; pBlockMap_->GetBlockType(targetPos) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.y = std::floor(centorTop.y) - 0.5f - pLocalBodyComp_->size_.y * 0.5f;
				velocity.y = 0;
				hitNormal.y = -1.f;
			}
		}
	}
	{ // 横方向
		{	// 左方向
			if (pBlockMap_->GetBlockType(centorLeft) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.x = std::ceil(centorLeft.x) - 0.5f + pLocalBodyComp_->size_.x * 0.5f;
				velocity.x = 0;
				hitNormal.x = 1.f;
			}
		}
		{	// 右方向
			if (pBlockMap_->GetBlockType(centorRight) != Block::BlockType::kNone) {
				pLocalBodyComp_->localPos_.x = std::floor(centorRight.x) - 0.5f - pLocalBodyComp_->size_.x * 0.5f;
				velocity.x = 0;
				hitNormal.x = -1.f;
			}
		}
	}

	// 接触した法線を返す
	hitNormal_ = hitNormal.Normalize();

	pRigidbody_->SetVelocity(velocity);


}
