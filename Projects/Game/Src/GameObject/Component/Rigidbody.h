#pragma once
#include "../GameObject.h"

#include "Math/Vector3.h"

class Rigidbody : public IComponent {
public:
	using IComponent::IComponent;
	~Rigidbody() = default;

	void Init();
	void Update() override;

	/// @brief 瞬間的な加速を行う
	/// @param vec 加速度
	void ApplyInstantForce(const Vector3 &vec) { acceleration_ += vec; }

	/// @brief 継続的な加速
	/// @param vec 加速度
	/// @param deltaTime 時間差分
	void ApplyContinuousForce(const Vector3 &vec) { acceleration_ += vec * GetDeltaTime(); }

	void SetAcceleration(const Vector3 &vec) { acceleration_ = vec; }
	const Vector3 &GetAcceleration() const { return acceleration_; }

	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }

	const Vector3 &GetVelocity() const { return velocity_; }

	bool hasCollider_ = false;
	float radius_ = 1.f;

	const Vector3 &GetBeforePos() const { return beforePos_; }

private:
	Vector3 velocity_{};
	Vector3 acceleration_{};

	Vector3 beforePos_;

	// Vector3 maxSpeed_{ -1.f,-1.f,-1.f };	// 負数である場合は処理しない

	// bool isGround_ = false;
};
