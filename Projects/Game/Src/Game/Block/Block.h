#pragma once
#include <cstdint>
#include <array>
#include "Math/Vector2.h"

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
		0x00FFFFFF,
	};
	inline static const std::array<Mat4x4, 3> kUvMatrix_{
		Mat4x4::MakeScalar({1.f / 3.f,1,1}),
		Mat4x4::MakeScalar({1.f / 3.f,1,1}) * Mat4x4::MakeTranslate({1.f / 3.f,0,0}),
		Mat4x4::MakeScalar({1.f / 3.f,1,1}) * Mat4x4::MakeTranslate({2.f / 3.f,0,0}),
	};

public:
	Block(BlockType type = BlockType::kNone) : blockType_(type) {}
	~Block() = default;

	Block &operator=(const BlockType type) { blockType_ = type; return *this; }

	operator bool() const { return blockType_ != BlockType::kNone and blockType_ != BlockType::kMax; }

	const BlockType GetBlockType() const { return blockType_; }

	void SetBlockType(const BlockType type) { blockType_ = type; }

	uint32_t GetColor() const { return kBlockColor_[static_cast<uint32_t>(blockType_)]; }

	int32_t GetDamage() const { return damage_; }

	int32_t AddDamage(const int32_t val) { damage_ += val;return damage_; }
	void SetDamage(const int32_t val) { damage_ = val; }

	bool GetHit() const { return isHit_; }

	//当たったブロックのフラグを変更
	void SetHitBlockFlug(const bool isHitFlug) {
		isHit_ = isHitFlug;
	}


	const Mat4x4 &GetDamageUv() const { return kUvMatrix_[damage_]; }

private:
	// ブロックの状態
	BlockType blockType_ = BlockType::kNone;
	int32_t damage_ = 0;

	bool isHit_ = false;
};