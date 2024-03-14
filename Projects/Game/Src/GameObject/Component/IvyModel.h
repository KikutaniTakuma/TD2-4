#pragma once
#include "../GameObject.h"

#include <memory>
#include <list>

#include "../LambEngine/Drawers/Line/Line.h"

class IvyModelComponent :public IComponent {
public:
	using IComponent::IComponent;

	~IvyModelComponent() = default;

	void Init() override;

	void Draw(const Camera &camera) const override;

	void AddLine(const Vector3 &begin, const Vector3 &end);

	/// @brief 終端の座標を返す
	/// @return 終端座標(もしnullptrなら存在しない)
	const Vector3 *GetLastPos() const;

private:

	std::list<std::unique_ptr<Line>> lines_;
};
