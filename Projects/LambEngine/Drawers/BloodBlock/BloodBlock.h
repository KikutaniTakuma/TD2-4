#pragma once
#pragma once
#include "../BaseDrawer.h"

#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 3Dモデルの描画
/// </summary>
class BloodBlock : public BaseDrawer {
public:
	struct ShaderData {
		Vector3 bloodVector; // パディング系でエラーが起きる可能性を考慮して3次元配列
		uint32_t isBlood;
	};

	// ブロックの最大描画数(128もあれば十分？)
	static constexpr uint32_t kMaxDrawCount = 128u;

	// シェーダータイプ
	using RenderContextType = RenderContext<ShaderData, kMaxDrawCount>;

	static const LoadFileNames kFileNames;

public:
	BloodBlock() = default;
	BloodBlock(const BloodBlock&) = default;
	BloodBlock(BloodBlock&& right) noexcept = default;
	virtual ~BloodBlock() = default;

	BloodBlock& operator=(const BloodBlock& right) = default;
	BloodBlock& operator=(BloodBlock&& right) noexcept = default;

public:
	virtual void Load();

	virtual void Draw(
		const Mat4x4& worldMatrix,
		const Mat4x4& camera,
		uint32_t color,
		BlendType blend,
		const Vector3& bloodVector,
		bool isBlood
	);
};