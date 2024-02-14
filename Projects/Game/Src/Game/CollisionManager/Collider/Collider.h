#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Drawers/Line/Line.h"
#include "Utils/Flg/Flg.h"
#include <bitset>
#include <memory>

/// <summary>
/// 当たり判定(AABB)
/// </summary>
class Collider {
public:
	Collider();
	Collider(const Collider&) = default;
	Collider(Collider&&) noexcept = default;
	~Collider() = default;

	Collider& operator=(const Collider&) = default;
	Collider& operator=(Collider&&) noexcept = default;

public:
	void UpdateCollision();

	/// <summary>
	/// ある一点が当たっているか
	/// </summary>
	/// <param name="pos">一点</param>
	/// <returns>当たっていたらtrue</returns>
	bool IsCollision(const Vector3& pos);

	/// <summary>
	/// 引数に取ったコライダーが当たっていたら引数側のコライダーを押し出す
	/// </summary>
	/// <param name="other">他のコライダー</param>
	/// <returns>当たっていたらtrue</returns>
	bool CollisionExtrusion(Collider& other);

	bool CollisionPush(Collider& other);

	void DebugDraw(const Mat4x4& viewProjection);

	void Debug(const std::string& guiName);

	void SetType(uint32_t type);

	bool Filter(const Collider& other) const;

	bool OnEnter() const{
		return flg_.OnEnter();
	}

	bool OnStay() const {
		return flg_.OnStay();
	}

	bool OnExit() const {
		return flg_.OnExit();
	}

	//void Adjusment(Collider& other, const Vector3& moveVec);

public:
	Vector3 scale_;
	Vector3 collisionPos_;

protected:
	Vector3 max_;
	Vector3 min_;

	uint32_t color_;

	std::array<std::unique_ptr<Line>, 12> lines_;

	std::bitset<32> types_;

	Lamb::Flg flg_;
};