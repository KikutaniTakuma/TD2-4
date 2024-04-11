#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Drawers/Line/Line.h"
#include "Utils/Flg/Flg.h"
#include <array>

/// <summary>
/// 当たり判定(OBB)
/// </summary>
class Obb {
public:
	Obb();
	Obb(const Obb&) = default;
	Obb(Obb&&) = default;
	~Obb() = default;

	Obb& operator=(const Obb&) = default;
	Obb& operator=(Obb&&) = default;

public:
	void Draw(const Mat4x4& viewProjection);

	void Update();

	void Debug(const std::string& guiName);

	bool IsCollision(Vector3 pos, float radius);

	bool OnEnter() const {
		return isCollision_.OnEnter();
	}

	bool OnStay() const {
		return isCollision_.OnStay();
	}

	bool OnExit() const {
		return isCollision_.OnExit();
	}

	Vector3 GetSize()const { return size_; }

	/// <summary>
	/// obbの回転を取得する
	/// </summary>
	/// <returns></returns>
	Mat4x4 GetRotate()const;
	/// <summary>
	/// obbの中に点があるかどうかの判定
	/// </summary>
	/// <param name="point">点の座標</param>
	/// <returns></returns>
	bool OBBinPoint(const Vector3& point);

	/// <summary>
	/// obb同士の当たり判定処理
	/// </summary>
	/// <param name="obb">当たるobb </param>
	/// <returns></returns>
	bool IsCollisionOBBOBB(const Obb& obb);

public:
	Vector3 center_;
	Vector3 scale_;
	Vector3 rotate_;

	uint32_t color_;

	std::array<Vector3, 3> orientations_;
private:
	std::array<Line, 12> lines_;
	std::array<Line, 3> orientationLines_;
	Vector3 size_;

	Mat4x4 worldMatrix_;

	Lamb::Flg isCollision_;

/// <summary>
/// ゲッター
/// </summary>
public:
	const Lamb::Flg& GetIsCollision() const {
		return isCollision_;
	}
};