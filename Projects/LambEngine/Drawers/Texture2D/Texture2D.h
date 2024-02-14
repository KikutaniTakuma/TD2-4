#pragma once
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"

#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include "Utils/Flg/Flg.h"

#include <array>

/// <summary>
/// 板ポリ描画
/// </summary>
class Texture2D {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

public:
	/// <summary>
	/// デフォルトのコンストラクタではwhite2x2.pngを読み込む
	/// </summary>
	Texture2D();
	Texture2D(const std::string& fileName);
	Texture2D(const Texture2D&);
	Texture2D(Texture2D&&) noexcept;
	~Texture2D();

	Texture2D& operator=(const Texture2D& right);
	Texture2D& operator=(Texture2D&& right) noexcept;

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	static void Initialize(
		const std::string& vsFileName = "./Resources/Shaders/Texture2DShader/Texture2D.VS.hlsl",
		const std::string& psFileName = "./Resources/Shaders/Texture2DShader/Texture2DNone.PS.hlsl"
	);

	static void Finalize();

private:
	static void LoadShader(const std::string& vsFileName, const std::string& psFileName);

	static void CreateGraphicsPipeline();

/// <summary>
/// 静的メンバ変数
/// </summary>
private:
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState_;
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineStateNoDepth_;
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineStateNoWrap_;
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineStateNoWrapNoDepth_;
	static Shader shader_;

	static D3D12_INDEX_BUFFER_VIEW indexView_;
	static Lamb::LambPtr<ID3D12Resource> indexResource_;


public:
	void LoadTexture(const std::string& fileName);
	void ThreadLoadTexture(const std::string& fileName);

public:
	void Update();

	void Draw(
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::Normal,
		bool isDepth = true,
		bool isWrap = true
	);

	void Debug(const std::string& guiName);

	bool Collision(const Vector2& pos2D) const;
	bool Collision(const Texture2D& tex2D) const;

	Vector2 GetTexSize() const {
		if (tex_) {
			return tex_->getSize();
		}
		else {
			return Vector2::kZero;
		}
	}

	void ChangeTexture(Texture* tex);

	/// <summary>
	/// アニメーションスタート関数
	/// </summary>
	/// <param name="aniUvPibot">アニメーションをスタートさせる場所</param>
	void AnimationStart(float aniUvPibot = 0.0f);

	/// <summary>
	/// アニメーション関数
	/// </summary>
	/// <param name="aniSpd">アニメーション速度(milliseconds)</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="aniUvStart">アニメーションをスタートさせる場所</param>
	/// <param name="aniUvEnd">アニメーションを終わらせる場所</param>
	void Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd);

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void AnimationPause();

	/// <summary>
	/// アニメーションを再スタート
	/// </summary>
	void AnimationRestart();

	/// <summary>
	/// アニメーションしているかを取得
	/// </summary>
	/// <returns>アニメーションフラグ</returns>
	bool GetIsAnimation()const {
		return isAnimation_;
	}

public:
	Vector2 scale;
	Vector3 rotate;
	Vector3 pos;

	Vector2 uvPibot;
	Vector2 uvSize;

	std::array<Vector3, 4> worldPos;

	uint32_t color;

	Lamb::Flg isSameTexSize;

	// テクスチャと同じスケールにしたときのスケール倍率
	float texScalar;

private:
	D3D12_VERTEX_BUFFER_VIEW vertexView_;
	Lamb::LambPtr<ID3D12Resource> vertexResource_;

	ConstBuffer<Mat4x4> wvpMat_;
	ConstBuffer<Vector4> colorBuf_;

	Texture* tex_;
	bool isLoad_;

	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;
public:
	float uvPibotSpd;
};