#pragma once
#include "../GameObject.h"
#include <list>
#include "../SoLib/Text/StaticString.h"

/// @brief 蔦コンポーネント
class IvyComponent : public IComponent {
public:
	using IComponent::IComponent;

	~IvyComponent() = default;

	void Init() override;

	void Update() override;

	void Draw(const Camera &vp) const override;

public:

	using GroupName = SoLib::Text::StaticString<"IvyComponent">;

private:


	std::list<std::unique_ptr<GameObject>> childIvys_;
	GameObject *parentIvys_;

};