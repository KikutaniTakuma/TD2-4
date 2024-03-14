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

void IvyModelComponent::AddLine(const Vector3 &begin, const Vector3 &end) {
	auto newLine = std::make_unique<Line>();
	newLine->start = begin;
	newLine->end = end;
	lines_.push_back(std::move(newLine));
}

const Vector3 *IvyModelComponent::GetLastPos() const
{
	Vector3 *result = nullptr;
	if (lines_.size()) {
		result = &lines_.back()->end;
	}

	return result;
}
