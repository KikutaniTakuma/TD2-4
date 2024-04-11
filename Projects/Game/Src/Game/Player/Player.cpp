#include "Player.h"

Player::~Player(){
	//for (auto& ptr : blockList_) {
	//	ptr.reset(); // ポインタの中身を解放する
	//}
}

void Player::Initialize(){
	CollisionManager_ = CollisionManager::GetInstance();

	input_ = Input::GetInstance();

	obb_ = std::make_unique<Obb>();

	obb_->scale_ = { 3.f,1.f,1.f };

}

void Player::Update(){
	blockList_.remove_if([&](const std::unique_ptr<Block>& block) {
		if (block->GetIsBreak()){
			boxCount_--;
			CollisionManager_->DeleteCollider(block->GetObb());
			return true;
		}
		return false;
	});

	obb_->Update();
	obb_->Debug("PLOBB");

	if (input_->GetKey()->Pushed(DIK_RETURN)){
		AddBox(obb_->center_);
	}

	std::list<std::unique_ptr<Block>>::iterator blockItr = blockList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*blockItr++)->Update();	// 次のモデルに移動
	}
}

void Player::Draw(const Camera& camera){
	obb_->Draw(camera.GetViewProjection());

	std::list<std::unique_ptr<Block>>::iterator blockItr = blockList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*blockItr++)->Draw(camera);	// 次のモデルに移動
	}
}

void Player::Debug(){
	ImGui::Begin("ボックス関連");
	ImGui::Text("ボックスの数 %d", static_cast<int>(boxCount_));

	ImGui::End();
}

void Player::AddBox(const Vector3& pos){
	newBlock_ = std::make_unique<Block>();
	newBlock_->Initialize();
	newBlock_->pos_ = pos;

	CollisionManager_->SetCollider(newBlock_->GetObb());

	blockList_.push_back(std::move(newBlock_));
	boxCount_++;
}

void Player::AllTrade(){
	std::list<std::unique_ptr<Block>>::iterator blockItr = blockList_.begin();

	// 箱の数分交換処理を行う
	for (size_t i = 0; i < boxCount_; i++) {
		(*blockItr++)->Trade();	
	}
}

