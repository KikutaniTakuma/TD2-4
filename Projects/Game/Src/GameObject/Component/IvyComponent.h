#pragma once
#include "../GameObject.h"
#include <list>
#include "../SoLib/Text/StaticString.h"
#include <cstdint>
#include "IvyModel.h"
#include "../SoLib/SoLib/SoLib_Timer.h"

/// @brief 蔦コンポーネント
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

	bool IsActive() const;

public:

	using GroupName = SoLib::Text::StaticString<"IvyComponent">;

private:

	Vector3 ivyDirections_ = { 0.f,10.f,0.f };
	Vector3 moveDirections_;

	SoLib::Time::DeltaTimer movingTime_{ 1.f };

	std::list<std::unique_ptr<GameObject>> childrenIvys_;
	GameObject *parentIvys_;

	IvyModelComponent *ivyModel_ = nullptr;

};