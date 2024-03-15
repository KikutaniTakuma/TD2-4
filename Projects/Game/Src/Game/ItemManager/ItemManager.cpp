#include "ItemManager.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"

void ItemManager::Initialize(){
	model_ = std::make_unique<Model>("./Resources/Ball.obj");
	model_->pos = { 1.5f,0.0f,0.0f };
	model_->scale = { 0.3f,0.3f,0.3f };

	model_->light.ptRange = 5.0f;
	model_->light.ptPos = model_->pos;
	model_->light.ptPos.y = 3.8f;
	model_->light.ptColor = Vector3::kIdentity * 15.0f;

	model2_ = std::make_unique<Model>("./Resources/Ball.obj");

	model2_->pos = { -1.5f,0.0f,0.0f };
	model2_->scale = { 1.0f,1.0f,1.0f };
	model2_->light.ptRange = 5.0f;
	model2_->light.ptPos = model2_->pos;
	model2_->light.ptPos.y = 3.8f;
	model2_->light.ptColor = Vector3::kIdentity * 15.0f;
	model2_->SetParent(model_.get());

	

	modelList_.push_back(std::move(model_));
	modelList_.push_back(std::move(model2_));

	sphere_.reset(new Sphere);

	stages_ = {
		"Stage1",
		"Stage2",
		"Stage3",
		"Stage4",
		"Stage5",
		"Stage6",
		"Stage7",
		"Stage8",
		"Stage9",
		"Stage10",
		"Stage11",
		"Stage12"
	};
	stageName_ = stages_[0].c_str();
}

void ItemManager::Finalize(){
	modelList_.clear();
}

void ItemManager::Update(){
	
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->get()->Update();
	}

	sphere_->Debug("Sphere");
	sphere_->Update();
}

void ItemManager::Draw(const Camera* camera){
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->get()->Draw(camera->GetViewProjection(), camera->GetPos());
	}
	sphere_->Draw(camera->GetViewProjection(), std::numeric_limits<uint32_t>::max());
}

void ItemManager::Debug(){
	ImGui::Begin("アイテムエディター", nullptr, ImGuiWindowFlags_MenuBar);
	int i = 0;

	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("オブジェクト追加")) {
			if (ImGui::TreeNode("アイテム追加")) {
				ImGui::DragFloat("大きさ", &sphere_->radius, 0.01f);
				//ImGui::DragFloat3("回転", &sphere_->worldRoate.x, 0.01f);
				ImGui::DragFloat3("座標", &sphere_->translation.x, 0.01f);

				if (ImGui::Button("アイテム追加")) {
					if (OperationConfirmation()) {
						AddItem(sphere_->translation, Vector3(sphere_->radius, sphere_->radius, sphere_->radius));
					}			
					
				}
				ImGui::TreePop();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("アイテムリスト")) {
			for (auto it = modelList_.begin(); it != modelList_.end();) {
				if (ImGui::TreeNode(("栄養[" + std::to_string(0 + i) + "]").c_str())) {
					ImGui::PushID(&*it);
					ImGui::DragFloat3("大きさ", &it->get()->scale.x, 0.01f);
					if (ImGui::IsItemActive()) {
						it->get()->color = 0xff0000ff;
						changeSize_ = true;
					}
					else {
						changeSize_ = false;
						it->get()->color = 0xffffffff;
					}
					ImGui::DragFloat3("座標", &it->get()->pos.x, 0.01f);
					if (changeSize_ == false) {
						if (ImGui::IsItemActive()) {
							it->get()->color = 0xff0000ff;
						}
						else {
							it->get()->color = 0xffffffff;
						}
					}
					if (ImGui::Button("このオブジェを削除")) {
						if (OperationConfirmation()){
							it = modelList_.erase(it);
						}
						else {
							++it;
						}
						
					}
					else {
						++it;
					}
					ImGui::PopID();
					ImGui::TreePop();
				}
				else {
					++it;
				}
				i++;
			}			

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ファイル関連")){
			for (size_t j = 0; j < stages_.size(); j++) {
				if (ImGui::RadioButton(stages_[j].c_str(), &stageSelectNum_, static_cast<int>(j))) {
					stageName_ = stages_[stageSelectNum_].c_str();
				}
			}
			if (ImGui::Button("ファイルに保存")){
				if (OperationConfirmation()) {
					SaveFile(stageName_);
				}
			}
			if (ImGui::Button("ファイルから読み込む")) {
				if (OperationConfirmation()) {
					LoadFiles(stageName_);
				}
			}
					
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	
	ImGui::End();
}

void ItemManager::AddItem(const Vector3& pos, const Vector3& scale){
	auto& model = modelList_.emplace_back(std::make_unique<Model>("./Resources/Ball.obj"));
	model->pos = pos;
	model->scale = scale;
}

bool ItemManager::OperationConfirmation() {
	int result = MessageBox(WindowFactory::GetInstance()->GetHwnd(), L"この操作を続けますか?", L"Confirmation", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES) {
		return true;
	}
	else if (result == IDNO) {
		return false;
	}
	else {
		return false;
	}

}

void ItemManager::SaveFile(const std::string& stageNumber){
	//保存
	json root;
	root = json::object();
	int i = 0;

	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		root[kItemName_][i]["Scale"] = json::array({
				it->get()->scale.x,
				it->get()->scale.y,
				it->get()->scale.z
			});
		root[kItemName_][i]["Rotate"] = json::array({
				it->get()->rotate.x,
				it->get()->rotate.y,
				it->get()->rotate.z
			});
		root[kItemName_][i]["Pos"] = json::array({
				it->get()->pos.x,
				it->get()->pos.y,
				it->get()->pos.z
			});
		std::filesystem::path dir(kDirectoryPath);
		if (!std::filesystem::exists(kDirectoryName)) {
			std::filesystem::create_directory(kDirectoryName);
		}
		// 書き込むjsonファイルのフルパスを合成する
		std::string filePath = kDirectoryPath + stageNumber + ".json";
		// 書き込み用ファイルストリーム
		std::ofstream ofs;
		// ファイルを書き込みように開く
		ofs.open(filePath);
		//ファイルオープン失敗
		if (ofs.fail()) {
			std::string message = "Failed open data file for write.";
			MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
			assert(0);
			break;
		}
		//ファイルにjson文字列を書き込む(インデント幅4)
		ofs << std::setw(4) << root << std::endl;
		//ファイルを閉じる
		ofs.close();
		i++;
	}

	std::string message = "File save completed.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
}

void ItemManager::FileOverWrite(){
	////読み込むjsonファイルのフルパスを合成する
	//std::string filePath = kDirectoryPath + kItemName_ + ".json";
	////読み込み用のファイルストリーム
	//std::ifstream ifs;
	////ファイルを読み込み用に開く
	//ifs.open(filePath);

	////上書き用に読み取り
	//json root;
	//ifs >> root;
	//ifs.close();

	//json overWrite;

	//int i = 0;
	//for (int i = 0; i < coralsWT_.size(); i++) {
	//	overWrite[kItemName_][i][0] = json::array({
	//			coralsWT_[i].scale_.x,
	//			coralsWT_[i].scale_.y,
	//			coralsWT_[i].scale_.z
	//		});
	//	overWrite[kItemName_][i][1] = json::array({
	//			coralsWT_[i].rotation_.x,
	//			coralsWT_[i].rotation_.y,
	//			coralsWT_[i].rotation_.z
	//		});
	//	overWrite[kItemName_][i][2] = json::array({
	//			coralsWT_[i].translation_.x,
	//			coralsWT_[i].translation_.y,
	//			coralsWT_[i].translation_.z
	//		});
	//}

	//root[kItemName_] = overWrite;

	//// 書き込み用ファイルストリーム
	//std::ofstream ofs;
	//// ファイルを書き込みように開く
	//ofs.open(filePath);
	////ファイルオープン失敗
	//if (ofs.fail()) {
	//	std::string message = "Failed open data file for write.";
	//	MessageBoxA(nullptr, message.c_str(), "Element", 0);
	//	assert(0);
	//	return;
	//}
	////ファイルにjson文字列を書き込む(インデント幅4)
	//ofs << std::setw(4) << root << std::endl;
	////ファイルを閉じる
	//ofs.close();

	//std::string message = "File overwriting completed.";
	//MessageBoxA(nullptr, message.c_str(), "Element", 0);
}

void ItemManager::ChackFiles(){
	if (!std::filesystem::exists(kDirectoryName)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (LoadChackItem(filePath.stem().string())) {
			chackOnlyNumber = 1;
		}

		if (fileName.size() != 0) {
			bool noneFail = true;
			for (size_t i = 0; i < fileName.size(); i++) {
				if (fileName[i].c_str() == filePath.stem().string()) {
					noneFail = false;
				}
			}
			if (noneFail) {
				fileName.push_back(filePath.stem().string());
			}

		}
		else {
			//ファイルの名前を取得
			fileName.push_back(filePath.stem().string());
		}
	}
}

void ItemManager::LoadFiles(const std::string& stageNumber){
	if (!std::filesystem::exists(kDirectoryName)) {
		std::string message = "This file path does not exist.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (filePath.stem().string() == stageNumber) {
			//ファイル読み込み
			LoadFile(filePath.stem().string());
			return;
		}
	}
	std::string message = "File not found.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
	
}

void ItemManager::LoadFile(const std::string& groupName){
	if (!LoadChackItem(groupName))
		return;
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	modelList_.clear();

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName_);
	//未登録チェック
	assert(itGroup != root.end());
	//各アイテムについて
	for (const auto& i : root[kItemName_]) {
		Vector3 newScale{};
		Vector3 newRotate{};
		Vector3 newPos{};
				
		from_json(i["Scale"], newScale);
		from_json(i["Rotate"], newRotate);
		from_json(i["Pos"], newPos);		
		
		AddItem(newPos, newScale);
	}
#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

bool ItemManager::LoadChackItem(const std::string& stageNumber){
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + stageNumber + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		ifs.close();
		return false;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName_);
	//未登録チェック
	if (itGroup != root.end()) {
		return true;
	}
	else {
		return false;
	}
}

void ItemManager::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
