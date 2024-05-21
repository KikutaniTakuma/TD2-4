#pragma once
#include <cstdint>
#include <array>
#include "Math/Vector2.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>

/// @brief ブロックのデータ。
class Block {
public:

	enum class BlockType : uint32_t {
		kNone,

		kRed,
		kBlue,
		kGreen,
		kYellow,

		kMax
	};

	inline static constexpr std::array<uint32_t, static_cast<uint32_t>(BlockType::kMax)> kBlockColor_{
		0x00000000,
		0xFF0000FF,
		0x0000FFFF,
		0x00FF00FF,
		0xFFFF00FF,
	};
	inline static const std::array<Mat4x4, 3> kUvMatrix_{
		Mat4x4::MakeScalar({1.f / 3.f,1,1}),
		Mat4x4::MakeScalar({1.f / 3.f,1,1}) * Mat4x4::MakeTranslate({1.f / 3.f,0,0}),
		Mat4x4::MakeScalar({1.f / 3.f,1,1}) * Mat4x4::MakeTranslate({2.f / 3.f,0,0}),
	};

	// ブロックのテクスチャ
	// 赤、青、緑、黄の四種(順番も)
	inline static  std::array<uint32_t, 4u> kTextures_{

	};

public:
	Block(BlockType type = BlockType::kNone) : blockType_(type) {}
	~Block() = default;

	static void StaticLoad() {
		kTextures_ = {
			TextureManager::GetInstance()->LoadTexture("Resources/BlockTex/lizardTailBlock.png"),
			TextureManager::GetInstance()->LoadTexture("Resources/BlockTex/waterBlock.png"),
			TextureManager::GetInstance()->LoadTexture("Resources/BlockTex/herbBlock.png"),
			TextureManager::GetInstance()->LoadTexture("Resources/BlockTex/mineralBlock.png"),
		};
	}

	Block &operator=(const BlockType type) { blockType_ = type; return *this; }

	operator bool() const { return blockType_ != BlockType::kNone and blockType_ != BlockType::kMax; }

	const BlockType GetBlockType() const { return blockType_; }

	void SetBlockType(const BlockType type) { blockType_ = type; }

	uint32_t GetColor() const { return kBlockColor_[static_cast<uint32_t>(blockType_)]; }

	int32_t GetDamage() const { return damage_; }

	int32_t AddDamage(const int32_t val) { damage_ += val; return damage_; }
	void SetDamage(const int32_t val) { damage_ = val; }

	const Mat4x4 &GetDamageUv() const { return kUvMatrix_[damage_]; }

	uint32_t GetTexture() const {
		if (blockType_ == BlockType::kNone) { return 0; }
		else
		{
			return kTextures_[static_cast<uint32_t>(blockType_) - 1];
		}
	}

	void SetIsDestroy(bool val) { isDestroy_ = val; }
	bool GetIsDestroy() const { return isDestroy_; }

private:	// 物理的にゲーム内に影響をもたらすデータ。
	// ブロックの状態
	BlockType blockType_ = BlockType::kNone;
	int32_t damage_ = 0;
	bool isDestroy_ = false;
};