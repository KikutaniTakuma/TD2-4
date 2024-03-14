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
	bool SplitIvy(int32_t splitCount);

	/// @brief モデルにデータを転送する
	void TransferData();

	/// @brief タイマーが動作中か
	/// @return 動いているならtrue
	bool IsActive() const;

	/// @brief ツタの線を追加
	void AddLine();

public:

	using GroupName = SoLib::Text::StaticString<"IvyComponent">;

private:
	Vector3 moveDirections_ = { 0.f,1.f,0.f };

	// 伸びる時間の初期値
	SoLib::VItem<"伸びる時間の初期値", float> vDefaultMoveTime_ = 1.f;

	// 伸びる時に使うタイマー
	SoLib::Time::DeltaTimer movingTime_{ };

	// 止まった後のタイマー
	SoLib::Time::DeltaTimer stopTime_{ };

	// 伸びる距離の初期値
	SoLib::VItem<"伸びる距離の初期値", float> vDefaultMaxLength_ = 5.f;

	// 子供のツタ
	std::list<std::unique_ptr<GameObject>> childrenIvys_;
	// 親のツタ
	IvyComponent *parentIvys_;

	// ツタのモデル
	IvyModelComponent *ivyModel_ = nullptr;

};