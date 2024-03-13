#include "IvyModel.h"

void IvyModelComponent::Init()
{
}

void IvyModelComponent::Draw(const Camera &camera) const
{
	for (const auto &line : lines_) {
		// ラインを表示
		line->Draw(camera.GetViewProjection());
	}
}
