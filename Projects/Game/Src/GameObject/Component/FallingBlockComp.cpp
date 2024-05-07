#include "FallingBlockComp.h"
#include "ModelComp.h"
#include <Drawers/DrawerManager.h>

void FallingBlockComp::Init()
{
	pLocalPos_ = object_.AddComponent<LocalBodyComp>();

	Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel("Resources/Cube.obj"))->color_ = 0xF58498FF;
}

void FallingBlockComp::Start()
{

}

void FallingBlockComp::Update()
{
	// 時間差分
	const float deltaTime = GetDeltaTime();

	// 加速度を加算
	velocity_ += gravity_ * deltaTime;
	// 移動量を加算
	pLocalPos_->localPos_ += velocity_ * deltaTime;

	// グローバル空間に持ってくる
	pLocalPos_->TransfarData();
}

std::list<Vector2> FallingBlockComp::FindLandingList() const
{

	std::list<Vector2> result;

	// 横方向の大きさ
	const Vector2 size = pLocalPos_->size_;
	const Vector2 halfOffset = (size - Vector2::kIdentity) / 2.f;

	Vector2 signVec;
	signVec.x = SoLib::Math::Sign(velocity_.x);
	signVec.y = SoLib::Math::Sign(velocity_.y);

	// for文で、縦横方向の大きさ分繰り返して対象のブロックを調べる
	for (float y = 0; y < size.y; y++)
	{
		for (float x = 0; x < size.x; x++)
		{
			Vector2 findPos = TargetPositon(signVec, Vector2{ x, y } - halfOffset);
			// 対象のブロックを調べて、虚空以外ならtrue
			if (pLocalPos_->pMap_->GetBoxType(findPos) != Block::BlockType::kNone)
			{
				result.push_back(findPos);
			}
		}
	}


	return result;

}
bool FallingBlockComp::IsLanding() const
{
	// 横方向の大きさ
	const Vector2 size = pLocalPos_->size_;
	// ブロックの半分の大きさ
	const Vector2 halfOffset = (size - Vector2::kIdentity) / 2.f;
	Vector2 signVec = Vector2::kZero;
	signVec.x = SoLib::Math::Sign(velocity_.x);
	signVec.y = SoLib::Math::Sign(velocity_.y);


	// もし範囲外なら着地していることにする
	if (BlockMap::IsOutSide(TargetPositon(signVec, -halfOffset)) || BlockMap::IsOutSide(TargetPositon(signVec, halfOffset)))
	{
		return true;
	}


	// for文で、縦横方向の大きさ分繰り返して移動先のブロックを調べる
	for (float y = 0; y < size.y; y++)
	{
		for (float x = 0; x < size.x; x++)
		{
			// 下のブロックを調べて、虚空以外ならtrue
			if (pLocalPos_->pMap_->GetBoxType(TargetPositon(signVec, Vector2{ x, y } - halfOffset)) != Block::BlockType::kNone)
			{
				return true;
			}
		}
	}

	// 範囲外に出てたらtrue、それ以外はfalse
	return pLocalPos_->localPos_.y - 1 < 0;
}