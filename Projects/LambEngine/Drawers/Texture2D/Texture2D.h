#pragma once
#include "../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"
#include <Math/Transform.h>

struct Tex2DState {
	Transform transform;//座標
	Transform uvTrnasform;//uv座標
	uint32_t textureID;//識別番号
	std::string textureName;//テクスチャの名前
	std::string textureFullPath;//テクスチャのフルパス
	uint32_t color;//色
};
/// <summary>
/// 板ポリ描画
/// </summary>
class Texture2D : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 4096u;

	
private:
	struct ShaderData{
		Mat4x4 uvTransform;
		Vector3 pad; // <- huh?
		uint32_t textureID = 0u;
	};


	using Texture2DRenderContext = RenderContext<ShaderData, kMaxDrawCount>;

	static const LoadFileNames kFileNames_;

public:
	Texture2D();
	Texture2D(const Texture2D&) = default;
	Texture2D(Texture2D&&) = default;
	~Texture2D() = default;

	Texture2D& operator=(const Texture2D&) = default;
	Texture2D& operator=(Texture2D&&) = default;

public:
	void Load();

	void Draw(
		const Mat4x4& worldMatrix, 
		const Mat4x4& uvTransform, 
		const Mat4x4& camera, 
		uint32_t textureID, 
		uint32_t color, 
		BlendType blend
	);
};