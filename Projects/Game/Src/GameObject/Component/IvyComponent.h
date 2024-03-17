#pragma once
#include "../GameObject.h"
#include <list>
#include "../SoLib/Text/StaticString.h"
#include <cstdint>
#include "IvyModel.h"
#include "../SoLib/SoLib/SoLib_Timer.h"

/// @brief ツタのコンポーネント
class IvyComponent : public IComponent {
public:
	using IComponent::IComponent;

	~IvyComponent() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera &vp) const override;

public:

	/// @brief ツタの分解を行う
	/// @return 分裂に成功したなら [true]
	bool SplitIvy(int32_t splitCount, uint32_t spritNumber);

	/// @brief モデルにデータを転送する
	void TransferData();

	/// @brief タイマーが動作中か
	/// @return 動いているならtrue
	bool IsActive() const;

	/// @brief ツタの線を追加
	void AddLine();

	const auto &GetLines() const { return ivyModel_->GetLines(); }

	const auto &GetChildren() const { return childrenIvys_; }

	std::list<const std::list<std::unique_ptr<Line>> *> GetAllLines();

	void SetPosIndex(uint32_t index) { posIndex = index; }
	uint32_t GetPosIndex() const { return posIndex; }

	void SetSplitNumber(uint32_t num) { splitNumber_ = num; }
	uint32_t GetSpritNumber() const { return splitNumber_; }

	GameObject *GetAllParent();

	IvyComponent *GetParent() { return parentIvys_; }

	/// @brief 子供が何世代いるか
	/// @return 子供の世代
	uint32_t GetChildGeneration() const;

public:

	using GroupName = SoLib::Text::StaticString<"IvyComponent">;

private:

	std::pair<uint32_t, float> deltaStore_{};

	bool isActive_ = false;

	Vector3 moveDirections_ = { 0.f,1.f,0.f };

	// 伸びる時間の初期値
	SoLib::VItem<"伸びる時間の初期値", float> vDefaultMoveTime_ = 0.8f;

	// 止まる時間の初期値
	SoLib::VItem<"止まる時間の初期値", float> vDefaultStopTime_ = 0.75f;

	// 伸びる時に使うタイマー
	SoLib::Time::DeltaTimer movingTime_{ };

	// 止まった後のタイマー
	SoLib::Time::DeltaTimer stopTime_{ };

	// 伸びる距離の初期値
	SoLib::VItem<"伸びる距離の初期値", float> vDefaultMaxLength_ = 150.f;

	SoLib::VItem<"初期発射角度", float> vDefaultAngle_ = 45_deg;

	// 子供のツタ
	std::list<std::unique_ptr<GameObject>> childrenIvys_;
	// 親のツタ
	IvyComponent *parentIvys_;

	// ツタのモデル
	IvyModelComponent *ivyModel_ = nullptr;

	// 存在してる座標
	uint32_t posIndex;

	// 分裂の番号
	uint32_t splitNumber_;

};