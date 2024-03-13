#pragma once
#include "../GameObject.h"
#include <list>
#include "../SoLib/Text/StaticString.h"
#include <cstdint>

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

public:

	using GroupName = SoLib::Text::StaticString<"IvyComponent">;

private:

	std::list<std::unique_ptr<GameObject>> childrenIvys_;
	GameObject *parentIvys_;

};