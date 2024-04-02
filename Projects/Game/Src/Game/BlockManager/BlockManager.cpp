#include "BlockManager.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"

void BlockManager::Initialize(){
	model_ = std::make_unique<Model>("./Resources/Cube.obj");
	model_->pos = { 1.5f,0.0f,0.0f };
	model_->scale = { 0.3f,0.3f,0.3f };

	model2_ = std::make_unique<Model>("./Resources/Cube.obj");
	model2_->pos = { -1.5f,0.0f,0.0f };
	model2_->scale = { 1.0f,1.0f,1.0f };
	//model2_->SetParent(model_.get());	

	modelList_.emplace(0,std::move(model_));
	modelList_.emplace(1,std::move(model2_));

	obb_ = std::make_unique<Obb>();
	obb_->scale_ = { 0.3f,0.3f,0.3f };

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

void BlockManager::Finalize(){
	modelList_.clear();
}

void BlockManager::Update(){
	
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		if (it->second->GetParent()) {
			it->second->scale = { 1.0f,1.0f,1.0f };
		}
		else {
			it->second->scale = { 0.5f,0.5f,0.5f };
		}
		it->second->Update();
	}
	obb_->scale_ = { boxSize_,boxSize_,boxSize_ };

	obb_->Debug("Sphere");
	obb_->Update();
}

void BlockManager::Draw(const Camera* camera){
	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		it->second->Draw(camera->GetViewProjection(), camera->GetPos());
	}
	obb_->Draw(camera->GetViewProjection());
}

void BlockManager::Debug(){
	/*Model* colorModel = nullptr;
	Model* colorModel2 = nullptr;*/
	ImGui::Begin("アイテムエディター", nullptr, ImGuiWindowFlags_MenuBar);
	int i = 0/*, k = 0*/;

	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("オブジェクト追加")) {
			if (ImGui::TreeNode("アイテム追加")) {
				ImGui::DragFloat("大きさ", &boxSize_, 0.01f);
				//ImGui::DragFloat3("回転", &sphere_->worldRoate.x, 0.01f);
				ImGui::DragFloat3("座標", &obb_->center_.x, 0.01f);

				if (ImGui::Button("アイテム追加")) {
					/*if (OperationConfirmation()) {*/
						AddBlock(obb_->center_, Vector3(obb_->scale_.x, obb_->scale_.y, obb_->scale_.z));
					//}			
					
				}
				ImGui::TreePop();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("アイテムリスト")) {
			for (auto it = modelList_.begin(); it != modelList_.end();) {
				if (ImGui::TreeNode(("栄養[" + std::to_string(0 + i) + "]").c_str())) {
					ImGui::PushID(&*it);
					ImGui::DragFloat3("大きさ", &it->second->scale.x, 0.01f);
					if (ImGui::IsItemActive()) {
						it->second->color = 0xff0000ff;
						changeSize_ = true;
					}
					else {
						changeSize_ = false;
						it->second->color = 0xffffffff;
					}
					ImGui::DragFloat3("座標", &it->second->pos.x, 0.01f);
					if (changeSize_ == false) {
						if (ImGui::IsItemActive()) {
							it->second->color = 0xff0000ff;
						}
						else {
							it->second->color = 0xffffffff;
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
	/*ImGui::Begin("親子関係エディター", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("アイテムリスト")) {
			i = 0;
			for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
				if (ImGui::TreeNode(("栄養[" + std::to_string(0 + i) + "]").c_str())) {
					
					
					k = 0;
					for (auto element = modelList_.begin(); element != modelList_.end(); ++element) {
						if (it == element ||
							it->second->GetParent() == element->second.get() ||
							element->second->GetParent() == it->second.get()) {
							k++;
							continue;
						}

						if (ImGui::Button(("栄養[" + std::to_string(0 + k) + "]を親にする").c_str())) {
							it->second->SetParent(element->second.get());
						}


						k++;
					}
					ImGui::TreePop();
				}
				i++;
			}

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("親子関係の確認")) {

			i = 0;
			ImGui::Text("親(赤)　＜＝　子(青)");
			for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
				k = 0;
				for (auto element = modelList_.begin(); element != modelList_.end(); ++element) {
					
					if (element->second->GetParent() == it->second.get()) {
						if (ImGui::MenuItem(("栄養[" + std::to_string(0 + i) + "]　＜＝　栄養[" + std::to_string(0 + k) + "]").c_str(), "この関係を解消する", isSelectImgui_)) {
							isSelectImgui_ = true;
							element->second->ClearParent();
							

						}
						if (ImGui::IsItemHovered()) {
							
							it->second->color = 0xff0000ff;
							element->second->color = 0x0000ffff;
							colorModel = it->second.get();
							colorModel2 = element->second.get();
						}
						else {
							if (colorModel != it->second.get() && colorModel != element->second.get()) {
								it->second->color = 0xffffffff;
								element->second->color = 0xffffffff;
							}
													
						}
						if (isSelectImgui_ == true){
							it->second->color = 0xffffffff;
							element->second->color = 0xffffffff;
							isSelectImgui_ = false;
						}
					}
					k++;
				}	
				i++;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	ImGui::End();*/

}


void BlockManager::AddBlock(const Vector3& pos, const Vector3& scale){
	model_ = std::make_unique<Model>("./Resources/Cube.obj");
	model_->pos = pos;
	model_->scale = scale;
	int t = static_cast<int>(modelList_.size());
	modelList_.emplace(t, std::move(model_));
}

bool BlockManager::OperationConfirmation() {
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

void BlockManager::SaveFile(const std::string& fileName){
	//保存
	json root;
	root = json::object();
	int i = 0;

	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
		//　保存する先のコンテナ
		auto& item = root[kItemName_][i];
		item["Scale"] = json::array({
				it->second->scale.x,
				it->second->scale.y,
				it->second->scale.z
			});
		item["Rotate"] = json::array({
				it->second->rotate.x,
				it->second->rotate.y,
				it->second->rotate.z
			});
		item["Pos"] = json::array({
				it->second->pos.x,
				it->second->pos.y,
				it->second->pos.z
			});

		std::filesystem::path dir(kDirectoryPath_);
		if (!std::filesystem::exists(kDirectoryName_)) {
			std::filesystem::create_directory(kDirectoryName_);
		}
		// 書き込むjsonファイルのフルパスを合成する
		std::string filePath = kDirectoryPath_ + fileName + ".json";
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

void BlockManager::ChackFiles(){
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

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
			chackOnlyNumber_ = 1;
		}

		if (fileName_.size() != 0) {
			bool noneFail = true;
			for (size_t i = 0; i < fileName_.size(); i++) {
				if (fileName_[i].c_str() == filePath.stem().string()) {
					noneFail = false;
				}
			}
			if (noneFail) {
				fileName_.push_back(filePath.stem().string());
			}

		}
		else {
			//ファイルの名前を取得
			fileName_.push_back(filePath.stem().string());
		}
	}
}

void BlockManager::LoadFiles(const std::string& fileName){
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::string message = "This file path does not exist.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (filePath.stem().string() == fileName) {
			//ファイル読み込み
			LoadFile(filePath.stem().string());
			return;
		}
	}
	std::string message = "File not found.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
	
}

void BlockManager::LoadFile(const std::string& fileName){
	if (!LoadChackItem(fileName))
		return;
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName + ".json";
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
		
		AddBlock(newPos, newScale);
	}
#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

bool BlockManager::LoadChackItem(const std::string& fileName){
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + fileName + ".json";
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

void BlockManager::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}

//void BlockManager::SaveFilePair(const std::string& fileName){
//	//保存
//	json root;
//	root = json::object();
//	int i = 0;
//
//	for (auto it = modelList_.begin(); it != modelList_.end(); ++it) {
//		root[kItemName_][i]["Scale"] = json::array({
//				it->second->scale.x,
//				it->second->scale.y,
//				it->second->scale.z
//			});
//		root[kItemName_][i]["Rotate"] = json::array({
//				it->second->rotate.x,
//				it->second->rotate.y,
//				it->second->rotate.z
//			});
//		root[kItemName_][i]["Pos"] = json::array({
//				it->second->pos.x,
//				it->second->pos.y,
//				it->second->pos.z
//			});
//
//		std::filesystem::path dir(kDirectoryPath_);
//		if (!std::filesystem::exists(kDirectoryName_)) {
//			std::filesystem::create_directory(kDirectoryName_);
//		}
//		// 書き込むjsonファイルのフルパスを合成する
//		std::string filePath = kDirectoryPath_ + fileName + ".json";
//		// 書き込み用ファイルストリーム
//		std::ofstream ofs;
//		// ファイルを書き込みように開く
//		ofs.open(filePath);
//		//ファイルオープン失敗
//		if (ofs.fail()) {
//			std::string message = "Failed open data file for write.";
//			MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
//			assert(0);
//			break;
//		}
//		//ファイルにjson文字列を書き込む(インデント幅4)
//		ofs << std::setw(4) << root << std::endl;
//		//ファイルを閉じる
//		ofs.close();
//		i++;
//	}
//
//	std::string message = "File save completed.";
//	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
//}
