#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"

class FallingBlockComp : public IComponent
{
public:
	using IComponent::IComponent;
	~FallingBlockComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

	// 中心座標
	Vector2 centerPos_{};
	// 直径
	Vector2 size_ = Vector2::kIdentity;
	// 速度
	Vector2 velocity_;
	// 重力
	Vector2 gravity_;
	// ダメージがあるか
	bool hasDamage_ = false;

private:
};