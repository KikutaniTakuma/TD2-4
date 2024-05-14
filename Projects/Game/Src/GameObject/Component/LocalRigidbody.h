#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"

class LocalRigidbody : public IComponent {
public:
	using IComponent::IComponent;

	void Init() override;
	void Update() override;

	/// @brief 瞬間的な加速を行う
	/// @param vec 加速度
	void ApplyInstantForce(const Vector2 vec) { acceleration_ += vec; }

	/// @brief 継続的な加速
	/// @param vec 加速度
	/// @param deltaTime 時間差分
	void ApplyContinuousForce(const Vector2 vec) { acceleration_ += vec * GetDeltaTime(); }

	void SetAcceleration(const Vector2 vec) { acceleration_ = vec; }
	const Vector2 GetAcceleration() const { return acceleration_; }

	void SetVelocity(const Vector2 vec) { velocity_ = vec; }

	const Vector2 GetVelocity() const { return velocity_; }

	const Vector2 GetBeforePos() const { return beforePos_; }

private:

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;

	Vector2 velocity_{};
	Vector2 acceleration_{};

	float friction_ = 0.95f;

	Vector2 beforePos_;
};
