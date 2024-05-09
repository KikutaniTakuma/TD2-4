#pragma once
#include <cstdint>
#include "../GameObject.h"
#include "LocalBodyComp.h"
#include "Utils/SafePtr/SafePtr.h"

enum class CollisionFilter : uint32_t {
	Player = 0b00001,
	Enemy = 0b00010,
	Bullet = 0b00100,
	Item = 0b01000,
	Ground = 0b10000,
};
// ビット演算用
CollisionFilter operator|(CollisionFilter l, CollisionFilter r) { return static_cast<CollisionFilter>(static_cast<uint32_t>(l) | static_cast<uint32_t>(r)); }
CollisionFilter operator&(CollisionFilter l, CollisionFilter r) { return static_cast<CollisionFilter>(static_cast<uint32_t>(l) & static_cast<uint32_t>(r)); }

class LocalColliderComp : public IComponent {
	// 衝突属性(自分自身が持つ属性)
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xFFFFFFFF;

	// 衝突無効化(相手が特定の属性を持っていた場合衝突を無効化)
	uint32_t collisionCancel_ = 0x0;

protected:

	Lamb::SafePtr<LocalBodyComp> pLocalBodyComp_;

public:
	// 親のコンストラクタを使う
	using IComponent::IComponent;

	/// @brief 衝突属性を取得
	/// @return 衝突属性(自分)
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	/// @brief 衝突属性を設定
	/// @param newAttribute 衝突属性(自分)
	void SetCollisionAttribute(const uint32_t newAttribute) { collisionAttribute_ = newAttribute; }
	/// @brief 衝突マスクを取得
	/// @return 衝突マスク(相手)
	uint32_t GetCollisionMask() const { return collisionMask_; }
	/// @brief 衝突マスクを設定
	/// @param newMask 衝突マスク(相手)
	void SetCollisionMask(const uint32_t newMask) { collisionMask_ = newMask; }

	/// @brief 衝突無効化フラグを取得
	/// @return 衝突無効化フラグ
	uint32_t GetCollisionCancel() const { return collisionCancel_; }
	/// @brief 衝突無効化フラグを設定
	/// @param newMask 衝突無効化フラグ
	void SetCollisionCancel(const uint32_t newFlag) { collisionCancel_ = newFlag; }

	/// @brief コライダの取得
	LocalBodyComp *GetCollider() { return pLocalBodyComp_.get(); }
	/// @brief コライダの取得
	const LocalBodyComp *GetCollider() const { return pLocalBodyComp_.get(); }

};