#include "Player.h"

Player::~Player(){
	//for (auto& ptr : blockList_) {
	//	ptr.reset(); // ポインタの中身を解放する
	//}
}

void Player::Initialize(){
	pCollisionManager_ = CollisionManager::GetInstance();


	input_ = Input::GetInstance();

	obb_ = std::make_unique<Obb>();

	obb_->center_.y = 15.0f;
	obb_->scale_ = { 3.f,1.f,1.f };

}

void Player::Update(){
	blockList_.remove_if([&](const std::unique_ptr<Block>& block) {
		if (block->GetIsBreak()){
			boxCount_--;
			pCollisionManager_->DeleteCollider(block->GetObb());
			return true;
		}
		return false;
	});

	PlayerMove();

	obb_->Update();
	obb_->Debug("PLOBB");
	

	std::list<std::unique_ptr<Block>>::iterator blockItr = blockList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*blockItr++)->Update();	// 次のモデルに移動
	}
}

void Player::Draw(const Camera& camera){
	obb_->Draw(camera.GetViewOthographics());

	std::list<std::unique_ptr<Block>>::iterator blockItr = blockList_.begin();

	// 箱の数分のモデルを描画する
	for (size_t i = 0; i < boxCount_; i++) {
		// モデルを描画
		(*blockItr++)->Draw(camera);	// 次のモデルに移動
	}
}

void Player::PlayerMove(){
	if (input_->GetKey()->Pushed(DIK_A)) {
		obb_->center_.x -= moveSpeed_;
	}
	if (input_->GetKey()->Pushed(DIK_D)) {
		obb_->center_.x += moveSpeed_;
	}
	if (input_->GetKey()->Pushed(DIK_S)) {
		obb_->center_.y -= moveSpeed_;
	}
	if (input_->GetKey()->Pushed(DIK_W)) {
		obb_->center_.y += moveSpeed_;
	}

	if (input_->GetKey()->Pushed(DIK_SPACE)) {
		AddBox(obb_->center_);
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

	pCollisionManager_->SetCollider(newBlock_->GetObb());

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

