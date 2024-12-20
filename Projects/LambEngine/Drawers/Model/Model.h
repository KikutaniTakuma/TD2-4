#pragma once
#include "../BaseDrawer.h"
#include <Math/Transform.h>

struct ModelState {
	Transform transform;
	uint32_t color;
};

/// @brief Matrix形式のモデルステータス
struct MatrixModelState {
	Mat4x4 transMat;
	uint32_t color;
};

/// <summary>
/// 3Dモデルの描画
/// </summary>
class Model : public BaseDrawer {
public:
	Model() = default;
	Model(const std::string& fileName);
	Model(const Model&) = default;
	Model(Model&& right) noexcept = default;
	virtual ~Model() = default;

	Model& operator=(const Model& right) = default;
	Model& operator=(Model&& right) noexcept = default;

public:
	virtual void Load(const std::string& fileName);

	virtual void Draw(
		const Mat4x4& worldMatrix,
		const Mat4x4& camera,
		uint32_t color,
		BlendType blend,
		bool isLighting = true
	);
};