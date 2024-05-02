#include "BlockGauge.h"

#include "Drawers/DrawerManager.h"

void BlockGauge::Init()
{
	gaugeDrawer_ = std::make_unique<GaugeDrawer>();
	gaugeDrawer_->Init();
}

void BlockGauge::Update(float deltaTime)
{
	gaugeDrawer_->SetParcent(energy_ / *vMaxEnergy_);
	gaugeDrawer_->Update(deltaTime);
}

void BlockGauge::Draw(const Camera &camera) const
{
	gaugeDrawer_->Draw(camera);
}

void BlockGauge::UseBlockEnergy(int32_t useBlockCount)
{
	// ブロック1個分の数
	const float oneBlockEnergy = vMaxEnergy_ / vMaxBlocks_;

	// 使用するエネルギーの数
	energy_ -= oneBlockEnergy * useBlockCount;
}

int32_t BlockGauge::GetBlockCount() const
{
	// ブロック1個分の数
	const float oneBlockEnergy = vMaxEnergy_ / vMaxBlocks_;

	// 1ブロック分のエネルギーが何個溜まっているか
	return static_cast<int32_t>(energy_ / oneBlockEnergy);
}

void BlockGauge::EnergyRecovery(float deltaTime)
{
	// エネルギーを回復する
	energy_ += vRecoveryEnergySec_ * deltaTime;
	// 範囲内に収める
	energy_ = std::clamp(energy_, 0.f, *vMaxEnergy_);

}



void GaugeDrawer::Init()
{
	Lamb::SafePtr manager = DrawerManager::GetInstance();
	manager->LoadModel(kModelName_);
	pModel_ = manager->GetModel(kModelName_);
}

void GaugeDrawer::Update([[maybe_unused]] float deltaTime)
{

	transform_.CalcMatrix();
}

void GaugeDrawer::Draw(const Camera &camera) const
{
	pModel_->Draw(transform_.matWorld_, camera.GetViewProjection(), 0xFF00FFFF, BlendType::kNone);
}

void GaugeDrawer::SetParcent(float parcent)
{
	transform_.scale.x = parcent * 10.f;
	transform_.translate = vCentorPos_;
	transform_.translate.x += parcent * -10.f;


}
