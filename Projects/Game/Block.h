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
		kPurple,

		kMax
	};

	inline static constexpr std::array<uint32_t, static_cast<uint32_t>(BlockType::kMax)> kBlockColor_{
		0x00000000,
		0xFF0000FF,
		0x0000FFFF,
		0x00FF00FF,
		0x00FFFFFF,
		0xFF00FFFF
	};

public:
	Block() = default;
	~Block() = default;

	operator bool() const { return blockType_ != BlockType::kNone and blockType_ != BlockType::kMax; }

	const BlockType GetBlockType() const { return blockType_; }

	void SetBlockType(const BlockType type) { blockType_ = type; }

private:
	// ブロックの状態
	BlockType blockType_ = BlockType::kNone;
};