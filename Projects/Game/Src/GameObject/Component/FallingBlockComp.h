#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"

class FallingBlockComp : public IComponent
{
public:
	using IComponent::IComponent;
	~FallingBlockComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

	Lamb::SafePtr<LocalBodyComp> pLocalPos_ = nullptr;

	// 速度
	Vector2 velocity_;
	// 重力
	Vector2 gravity_;
	// ダメージがあるか
	bool hasDamage_ = false;

private:
};