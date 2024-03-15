#include "IvyComponent.h"
#include "IvyModel.h"
#include "../SoLib/Math/Euler.h"
#include "../SoLib/Math/Math.hpp"

void IvyComponent::Init() {
	// モデルを紐づけ
	ivyModel_ = object_.AddComponent<IvyModelComponent>();
	movingTime_.Start(vDefaultMoveTime_);
	isActive_ = true;
}

void IvyComponent::Update() {
	const float deltaTime = GetDeltaTime();
	movingTime_.Update(deltaTime);
	stopTime_.Update(deltaTime);

	for (auto &child : childrenIvys_) {
		child->Update(deltaTime);
	}

	// 動作中タイマーが有効で、&
	// 子供が存在しない
	// 場合は線を追加する
	if (movingTime_.IsActive() and not childrenIvys_.size()) {
		// 線の追加
		AddLine();
		// 終了している(瞬間)に停止タイマーを動かす
		if (movingTime_.IsFinish()) {
			stopTime_.Start(vDefaultStopTime_);
		}
	}

	if (stopTime_.IsFinish()) {
		isActive_ = false;
	}
}

void IvyComponent::Draw([[maybe_unused]] const Camera &vp) const {
	for (const auto &child : childrenIvys_) {
		child->Draw(vp);
	}
}

bool IvyComponent::SplitIvy(int32_t splitCount) {
	// もし分裂数が残ってないならその場で終了
	if (splitCount <= 0) { return false; }

	// 分裂に成功したなら true
	bool result = true;

	// 子供を持っている場合子供に渡す
	if (childrenIvys_.size()) {
		// 全ての子供に分裂処理を行う
		for (auto &child : childrenIvys_) {
			// 分裂に失敗したら false
			result &= child->GetComponent<IvyComponent>()->SplitIvy(splitCount - 1);
		}
	}
	// 持っていない場合は追加
	else {

		// 動作中をフラグを無効化
		isActive_ = false;

		// 線の開始地点の取得
		const Vector3 *lastPos = ivyModel_->GetLastPos();
		// もし開始地点が存在しない場合は原点から
		if (not lastPos) {
			// 自分自身の座標を渡す
			lastPos = &transform_.translate;
		}

		for (uint32_t i = 0; i < 2; i++) {
			auto child = std::make_unique<GameObject>();
			auto childIvy = child->AddComponent<IvyComponent>();
			// 自分自身を親として保存
			childIvy->parentIvys_ = this;

			// 終端の座標を子供に渡す
			child->transform_.translate = *lastPos;

			// 自分の角度から45度回して子供に渡す
			childIvy->moveDirections_ = moveDirections_ * Quaternion::MakeRotateZAxis(i ? -vDefaultAngle_ : *vDefaultAngle_);

			// 子供のコンテナに格納
			childrenIvys_.push_back(std::move(child));
		}
	}

	return result;
}

void IvyComponent::TransferData() {

}

bool IvyComponent::IsActive() const {
	// 自分自身か、子供のどちらかがtrueであればtrueを返す
	return isActive_ or [this]()->bool
		{
			for (const auto &child : GetChildren()) {
				// どれか一つでもtrueであった場合はtrueを返す
				if (child->GetComponent<IvyComponent>()->IsActive()) {
					return true;
				}
			}
			// 全てがtrueであればtrueを返す
			return false;
		}();
}

void IvyComponent::AddLine() {
	if (deltaStore_.first++ < 5) {
		deltaStore_.second += GetDeltaTime();
		return;
	}
	deltaStore_.first = 0;

	// 線の開始地点の取得
	const Vector3 *lastPos = ivyModel_->GetLastPos();
	// もし開始地点が存在しない場合は原点から
	if (not lastPos) {
		lastPos = &transform_.translate;
	}
	// 追加される線の長さ
	float lineLength = static_cast<float>(deltaStore_.second / *vDefaultMoveTime_) * vDefaultMaxLength_;
	// 線の追加
	ivyModel_->AddLine(*lastPos, *lastPos + moveDirections_ * lineLength);

	deltaStore_.second = 0.f;
}

std::list<const std::list<std::unique_ptr<Line>> *> IvyComponent::GetAllLines()
{
	std::list<const std::list<std::unique_ptr<Line>> *> result{};
	result.push_back(&GetLines());

	for (const auto &child : childrenIvys_) {
		result.splice(result.end(), std::move(child->GetComponent<IvyComponent>()->GetAllLines()));
	}

	return result;
}
