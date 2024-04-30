#pragma once
#include "../GameObject.h"
#include "Utils/SafePtr/SafePtr.h"
#include "LocalBodyComp.h"
#include "PickUpComp.h"
#include "SoLib/Math/Math.hpp"

class DwarfComp : public IComponent
{
public:
	// 移動速度
	inline static const float kMovementSpeed_ = 1.0f;
	// 移動の減少値
	inline static const float kMovementResistance_ = 0.1f;

public:
	using IComponent::IComponent;
	~DwarfComp() = default;

	void Init() override;

	void Start() override;

	void Update() override;

public:

private:


	/// <summary>
	/// 移動を行う
	/// </summary>
	/// <param name="target">移動先</param>
	/// <returns>ゴールに到達したか</returns>
	bool MoveBlockPos(Vector2 target)
	{
		// ターゲットが指定されていない場合終わり
		if (target == -Vector2::kIdentity) { return false; }

		float xMove = target.x - pLocalBodyComp_->localPos_.x;
		pLocalBodyComp_->localPos_.x += std::clamp(xMove, -1.f, 1.f) * 5.f * GetDeltaTime() * (kMovementSpeed_ - kMovementResistance_ * pPickUpComp_->GetBlockWeight());

		// 移動距離が有効なら
		if (xMove != 0)
		{
			// 方向を与える
			facing_ = static_cast<int32_t>(SoLib::Math::Sign(xMove));
		}

		return (target - pLocalBodyComp_->localPos_ + Vector2::kYIdentity).Length() < 0.1f;
	}

private:

	int32_t facing_;

	Vector2 targetPos_;

	bool isClimbing_ = false;

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_ = nullptr;
	Lamb::SafePtr<PickUpComp> pPickUpComp_ = nullptr;

};