#include "IvyComponent.h"
#include "IvyModel.h"

void IvyComponent::Init() {
	// モデルを紐づけ
	ivyModel_ = object_.AddComponent<IvyModelComponent>();
	movingTime_.Start(vDefaultMoveTime_);
}

void IvyComponent::Update() {
	const float deltaTime = GetDeltaTime();
	movingTime_.Update(deltaTime);

	for (auto &child : childrenIvys_) {
		child->Update(deltaTime);
	}

	// タイマーが有効な間は線を追加する
	if (movingTime_.IsActive()) {
		// 線の追加
		AddLine();
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
		for (uint32_t i = 0; i < 2; i++) {
			auto child = std::make_unique<GameObject>();
			auto childIvy = child->AddComponent<IvyComponent>();
			// 自分自身を親として保存
			childIvy->parentIvys_ = this;

			childrenIvys_.push_back(std::move(child));
		}
	}

	return result;
}

void IvyComponent::TransferData() {

}

bool IvyComponent::IsActive() const {

	return movingTime_.IsActive();
}

void IvyComponent::AddLine() {
	// 線の開始地点の取得
	const Vector3 *lastPos = ivyModel_->GetLastPos();
	// もし開始地点が存在しない場合は原点から
	if (not lastPos) {
		lastPos = &transform_.translate;
	}
	// 追加される線の長さ
	float lineLength = static_cast<float>(GetDeltaTime() / *vDefaultMoveTime_) * vDefaultMaxLength_;
	// 線の追加
	ivyModel_->AddLine(*lastPos, *lastPos + moveDirections_ * lineLength);
}
