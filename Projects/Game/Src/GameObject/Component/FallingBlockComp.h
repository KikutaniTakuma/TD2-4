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
	std::list<Vector2> FindLandingList() const;

	bool IsLanding() const;

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