#include "FallingBlockComp.h"
#include "ModelComp.h"
#include <Drawers/DrawerManager.h>
#include "PlayerComp.h"

void FallingBlockComp::Init()
{
	pLocalPos_ = object_.AddComponent<LocalBodyComp>();
	pRigidbody_ = object_.AddComponent<LocalRigidbody>();
	pHitMapComp_ = object_.AddComponent<LocalMapHitComp>();

	Lamb::SafePtr modelComp = object_.AddComponent<ModelComp>();
	modelComp->AddBone("Body", DrawerManager::GetInstance()->GetModel("Resources/Cube.obj"))->color_ = 0xF58498FF;
}

void FallingBlockComp::Start()
{
	Lamb::SafePtr modelComp = object_.GetComponent<ModelComp>();
	modelComp->GetBone("Body")->color_ = blockType_.GetColor();
}

void FallingBlockComp::Update()
{
	// 時間差分
	//const float deltaTime = GetDeltaTime();

	pRigidbody_->ApplyContinuousForce(gravity_);

	// グローバル空間に持ってくる
	pLocalPos_->TransfarData();
}

void FallingBlockComp::OnCollision(GameObject *other)
{
	Lamb::SafePtr playerComp = other->GetComponent<PlayerComp>();
	if (playerComp) {
		playerComp->InflictDamage(1);
	}

}

bool FallingBlockComp::IsLanding() const
{
	//// 横方向の大きさ
	//const Vector2 size = pLocalPos_->size_;
	//// ブロックの半分の大きさ
	//const Vector2 halfOffset = (size - Vector2::kIdentity) / 2.f;
	//Vector2 signVec = Vector2::kZero;
	//signVec.x = SoLib::Math::Sign(velocity_.x);
	//signVec.y = SoLib::Math::Sign(velocity_.y);


	//// もし範囲外なら着地していることにする
	//if (BlockMap::IsOutSide(TargetPositon(signVec, -halfOffset)) || BlockMap::IsOutSide(TargetPositon(signVec, halfOffset)))
	//{
	//	return true;
	//}


	//// for文で、縦横方向の大きさ分繰り返して移動先のブロックを調べる
	//for (float y = 0; y < size.y; y++)
	//{
	//	for (float x = 0; x < size.x; x++)
	//	{
	//		// 下のブロックを調べて、虚空以外ならtrue
	//		if (pLocalPos_->pMap_->GetBlockType(TargetPositon(signVec, Vector2{ x, y } - halfOffset)) != Block::BlockType::kNone)
	//		{
	//			return true;
	//		}
	//	}
	//}

	//// 範囲外に出てたらtrue、それ以外はfalse
	//return pLocalPos_->localPos_.y - 1 < 0;

	return pHitMapComp_->hitNormal_ != Vector2::kZero;
}