#pragma once
#include "Drawers/DrawerManager.h"

#include <memory>
#include <cmath>

class NumberSprite {
private:
	static uint32_t numberTextureID_;

public:
	static void Load();

public:
	NumberSprite();
	NumberSprite(const NumberSprite& right) = default;
	NumberSprite(NumberSprite&& right) = default;
	~NumberSprite() = default;


public:
	NumberSprite& operator=(const NumberSprite& right);
	NumberSprite& operator=(NumberSprite&& right) noexcept;

public:
	Transform& GetTransform() {
		return *transform_;
	}
	const Transform& GetTransform() const {
		return *transform_;
	}

	void SetNumber(uint32_t num) {
		num_ = num;
		uint32_t digitTmp = static_cast<uint32_t>(std::log10(num_)) + 1u;
		if (digitNum_ < digitTmp) {
			digitNum_ = digitTmp;
		}
	}
	void SetNumberAndDigit(uint32_t num) {
		num_ = num;
		digitNum_ = static_cast<uint32_t>(std::log10(num_)) + 1;
	}
	uint32_t GetNumber() const {
		return num_;
	}
	void SetDigitNum(uint32_t digitNum) {
		digitNum_ = digitNum;
	}

	void Update();

	void Draw(const class Camera& camera, BlendType blendTyoe = BlendType::kNormal);

	void ImGuiWidget(const std::string& guiName = "");

private:
	std::unique_ptr<Transform> transform_;
	uint32_t num_;
	uint32_t digitNum_;
};

/// <summary>
/// 数字から特定の桁の数を抽出する
/// </summary>
uint32_t NumberToDigitNumber(uint32_t num, uint32_t digit);