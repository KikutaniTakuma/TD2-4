#pragma once
#include "Vector3.h"
#include "Mat4x4.h"
#include "Quaternion.h"

#include <d3d12.h>
#include <wrl.h>

/// @brief Transformクラス
struct Transform {
	Transform &operator=(const Transform &other) = default;

	const Transform *parent_ = nullptr;	// 親へのアドレス
	Vector3 scale = { 1.f,1.f,1.f };
	Vector3 translate{};
	Quaternion rotate = Quaternion::kIdentity;
	Mat4x4 matWorld_ = Mat4x4::Identity();	// ローカル . ワールド変換

public:

	const Vector3 &GetGrobalPos() const { return *reinterpret_cast<const Vector3 *const>(&matWorld_[3][0]); }

	Mat4x4 MakeAffine() const;

	void CalcMatrix();

	bool ImGuiWidget(const std::string &name = "");
	bool ImGuiWidget2D();

	// void MatToSRT(const Mat4x4 &mat);

	/// @brief 純粋な親子関係の構築
	/// @param parent 親のTransform
	void SetParent(const Transform &parent);

	/// @brief グローバル座標を維持した親子関係の構築
	/// @param parent 親のTransform
	// void ConnectParent(const Transform &parent);

	/// @brief グローバル座標を維持した親子関係の破棄
	// void DisConnectParent();
private:
};

struct SimpleTransform {
	Vector3 scale = Vector3::kIdentity;
	Vector3 translate{};
	Quaternion rotate = Quaternion::kIdentity;

	Mat4x4 MakeAffine() const;

};