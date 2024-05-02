#pragma once
#include "../GameObject.h"
#include "Drawers/Texture2D/Texture2D.h"

class SpriteComp : public IComponent
{
public:
	using IComponent::IComponent;
	~SpriteComp() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera &camera) const override;

public:

	/// @brief テクスチャを設定する
	/// @param textureID テクスチャの添え字
	void SetTexture(uint32_t textureID);

	/// @brief テクスチャを設定する
	/// @param textureID テクスチャの添え字
	void SetTexture(const std::string &filePath);

	/// @brief テクスチャのUV座標の行列計算
	void CalcTexUv();

public:

	SimpleTransform uvTransform_;

	Mat4x4 uvMatrix_;

	// スプライト
	inline static Lamb::SafePtr<Texture2D> pTexture_ = nullptr;

	uint32_t textureID_;

	uint32_t color_ = 0xFFFFFFFF;

	BlendType blendType_ = BlendType::kNormal;

private:

};


