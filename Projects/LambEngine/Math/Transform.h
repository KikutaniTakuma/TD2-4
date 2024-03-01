#pragma once
#include "Vector3.h"
#include "Mat4x4.h"

#include <d3d12.h>
#include <wrl.h>

/// @brief Transformクラス
struct BaseTransform {
	BaseTransform &operator=(const BaseTransform &other) = default;

	Vector3 scale = { 1.f,1.f,1.f };
	Vector3 rotate{};
	Vector3 translate{};

	Mat4x4 matWorld_{};	// ローカル . ワールド変換
	const BaseTransform *parent_ = nullptr;	// 親へのアドレス

public:

	const Vector3 &GetGrobalPos() const { return *reinterpret_cast<const Vector3 *const>(matWorld_.m[3].data()); }

	Mat4x4 MakeAffine() const;

	void CalcMatrix();
	void UpdateMatrix();

	bool ImGuiWidget(const std::string &name = "");
	bool ImGuiWidget2D();

	void MatToSRT(const Mat4x4 &mat);

	/// @brief 純粋な親子関係の構築
/// @param parent 親のTransform
	void SetParent(const BaseTransform &parent);

	/// @brief グローバル座標を維持した親子関係の構築
	/// @param parent 親のTransform
	void ConnectParent(const BaseTransform &parent);

	/// @brief グローバル座標を維持した親子関係の破棄
	void DisConnectParent();
private:
};

struct Transform :public BaseTransform {



};