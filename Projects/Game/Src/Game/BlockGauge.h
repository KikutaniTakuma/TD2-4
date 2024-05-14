#pragma once
#include <cstdint>
#include <memory>

#include "Camera/Camera.h"
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "SoLib/Containers/VItem.h"

class GaugeDrawer {
public:
	GaugeDrawer() = default;
	~GaugeDrawer() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera& camera) const;

	void SetParcent(float parcent);

	SoLib::VItem<"ゲージ描画の中心座標", Vector3> vCentorPos_ = Vector3{ 15.f,23.f,0.f };

private:


	inline static const char* const kModelName_ = "Resources/Cube.obj";

	Model* pModel_ = nullptr;

	Transform transform_;

};

class BlockGauge {
public:
	BlockGauge() = default;
	~BlockGauge() = default;

	void Init();

	void Update(float deltaTime);

	void Draw(const Camera &camera) const;

	void UseBlockEnergy(int32_t useBlockCount);

	/// @brief ブロック何個分が溜まっているか
	/// @return 溜まっているブロックの数
	int32_t GetBlockCount() const;

	/// @brief エネルギーを回復する
	/// @param deltaTime 時間差分
	void EnergyRecovery(float deltaTime);

private:


	SoLib::VItem<"エネルギーの最大値", float> vMaxEnergy_ = 100;

	SoLib::VItem<"一秒間に回復するエネルギーの量", float> vRecoveryEnergySec_ = 20;

	SoLib::VItem<"ブロックの最大量", int32_t> vMaxBlocks_ = 6;

	// エネルギーの量
	float energy_ = *vMaxEnergy_;

	// 描画用
	std::unique_ptr<GaugeDrawer> gaugeDrawer_;

};

