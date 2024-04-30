#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"
#include "SoLib/Math/Math.hpp"

class FallingBlockComp : public IComponent
{
public:
	using IComponent::IComponent;
	~FallingBlockComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

public:

	/// @brief ブロックの重さを返す
	/// @return ブロックの重さ
	int32_t GetWeight() const {
		return static_cast<int32_t>(pLocalPos_->size_.x * pLocalPos_->size_.y);
	}


	Vector2 TargetPositon(Vector2 move, Vector2 offset) const
	{
		return  pLocalPos_->localPos_ + offset + move;
	}


	bool IsLanding() const
	{
		// 横方向の大きさ
		const Vector2 size = pLocalPos_->size_;
		// ブロックの半分の大きさ
		const Vector2 halfSize = (size - Vector2::kIdentity) / 2.f;
		Vector2 signVec = Vector2::kZero;
		signVec.x = SoLib::Math::Sign(velocity_.x);
		signVec.y = SoLib::Math::Sign(velocity_.y);


		// もし範囲外なら着地していることにする
		if (Map::IsOutSide(TargetPositon(signVec, -halfSize)) || Map::IsOutSide(TargetPositon(signVec, halfSize)))
		{
			return true;
		}


		// for文で、横方向の大きさ分繰り返して移動先のブロックを調べる
		for (float y = 0; y < size.y; y++)
		{
			for (float x = 0; x < size.x; x++)
			{
				// 下のブロックを調べて、虚空以外ならtrue
				if (pLocalPos_->pMap_->GetBoxType(TargetPositon(signVec, Vector2{ x, y } - halfSize)) != Map::BoxType::kNone)
				{
					return true;
				}
			}
		}

		// 範囲外に出てたらtrue、それ以外はfalse
		return pLocalPos_->localPos_.y - 1 < 0;
	}
public:

	// ローカル座標コンポーネント
	Lamb::SafePtr<LocalBodyComp> pLocalPos_ = nullptr;

	// 速度
	Vector2 velocity_;
	// 重力
	Vector2 gravity_;
	// ダメージがあるか
	bool hasDamage_ = false;

private:
};