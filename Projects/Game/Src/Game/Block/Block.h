#pragma once
#include <cstdint>
#include <array>
#include "Math/Vector2.h"
#include <Engine/Graphics/TextureManager/TextureManager.h>
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"


/// @brief ブロックのデータ。
class Block {
public:

	enum class BlockType : uint32_t {
		kNone,

		kRed,
		kBlue,
		kGreen,
		kYellow,
		kPurple,

		kMax
	};

	inline static constexpr std::array<uint32_t, static_cast<uint32_t>(BlockType::kMax)> kBlockColor_{
		0x00000000,
		0xFF0000FF,
		0x0000FFFF,
		0x00FF00FF,
		0xFFFF00FF,
		0xFF00FFFF,
	};
	inline static const std::array<Mat4x4, 3> kUvMatrix_{
		Mat4x4::MakeScalar(Vector3{1.f / 3.f,1,1}),
		Mat4x4::MakeScalar(Vector3{1.f / 3.f,1,1}) * Mat4x4::MakeTranslate(Vector3{1.f / 3.f,0,0}),
		Mat4x4::MakeScalar(Vector3{1.f / 3.f,1,1}) * Mat4x4::MakeTranslate(Vector3{2.f / 3.f,0,0}),
	};

	// ブロックのテクスチャ
	// 赤、青、緑、黄、紫の五種(順番も)
	inline static  std::array<uint32_t, static_cast<uint32_t>(BlockType::kMax) - 1> kTextures_{

	};


	// アイテムのテクスチャ
	// 赤、青、緑、黄、紫の五種(順番も)
	inline static  std::array<uint32_t, static_cast<uint32_t>(BlockType::kMax) - 1> kItemTexture{

	};

public:
	Block(BlockType type = BlockType::kNone) : blockType_(type) {}
	~Block() = default;

	static void StaticLoad() {
		auto *textureManager = TextureManager::GetInstance();
		kTextures_ = {
			textureManager->LoadTexture("./Resources/BlockTex/powderBlock.png"),
			textureManager->LoadTexture("./Resources/BlockTex/waterBlock.png"),
			textureManager->LoadTexture("./Resources/BlockTex/herbBlock.png"),
			textureManager->LoadTexture("./Resources/BlockTex/mineralBlock.png"),
			textureManager->LoadTexture("./Resources/BlockTex/lizardTailBlock.png"),
		};
		kItemTexture = {
			textureManager->LoadTexture("./Resources/Item/powder.png"),
			textureManager->LoadTexture("./Resources/Item/water.png"),
			textureManager->LoadTexture("./Resources/Item/herbs.png"),
			textureManager->LoadTexture("./Resources/Item/mineral.png"),
			textureManager->LoadTexture("./Resources/Item/lizardTail.png"),
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

	/// @brief アイテムのテクスチャを取ってくる
	/// @param blockType ブロックのタイプ
	/// @return テクスチャのID
	static uint32_t GetItemTexture(const BlockType blockType) {
		if (blockType == BlockType::kNone) { return 0; }
		else
		{
			return kItemTexture[static_cast<uint32_t>(blockType) - 1];
		}
	}

	uint32_t GetItemTexture() const {
		return GetItemTexture(blockType_);
	}

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