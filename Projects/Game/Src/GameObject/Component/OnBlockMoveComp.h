#pragma once
#include "../GameObject.h"
#include "Game/Map/Map.h"

class OnBlockMoveComp : public IComponent
{
public:

	inline static constexpr uint32_t kJumpLength = 2u;

public:
	using IComponent::IComponent;

	~OnBlockMoveComp() override = default;

	void Init() override;

	void Update() override;

public:

	/// @brief 移動方向を指定
	/// @param dir 方向ベクトル
	void InputMoveDirection(const Vector3 &dir) { moveDir_ = dir; moveDir_.z *= -1.f; }

private:
	/// @brief 着地しているか
	/// @return 着地している場合true
	bool CheckLanding() const;

	/// @brief 通常の移動ができるかどうか
	/// @return できる場合true
	bool CheckNormalMove() const;

	/// @brief 飛び越える移動ができるかどうか
	/// @return できる場合true
	bool CheckJumpSideMove() const;

private:

	const BlockMap *pMap_;

	Vector3 moveDir_;

	// ステージ上の座標
	Vector3 onStagePos_;
};