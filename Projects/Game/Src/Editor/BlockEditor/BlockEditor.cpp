#include "BlockEditor.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"

void BlockEditor::Initialize(){

	map_ = GameManager::GetInstance()->GetMap();

	mapSize_ = map_->GetBlockMap();

	beforeMapSize_ = *mapSize_;

	input_ = Input::GetInstance();

	obb_ = std::make_unique<Obb>();

	obb_->scale_ = { 1.f,1.f,1.f };
	
}

void BlockEditor::Finalize(){

}

void BlockEditor::Update(){

	if (input_->GetKey()->LongPush(DIK_LSHIFT)&& input_->GetKey()->Pushed(DIK_0)){
		selectFloor_ = 0;

	}
	else if (input_->GetKey()->LongPush(DIK_LSHIFT) && input_->GetKey()->Pushed(DIK_1)) {
		selectFloor_ = 1;
	}
	else if (input_->GetKey()->LongPush(DIK_LSHIFT) && input_->GetKey()->Pushed(DIK_2)) {
		selectFloor_ = 2;
	}
	else if (input_->GetKey()->LongPush(DIK_LSHIFT) && input_->GetKey()->Pushed(DIK_3)) {
		selectFloor_ = 3;
	}
	else if (input_->GetKey()->LongPush(DIK_LSHIFT) && input_->GetKey()->Pushed(DIK_4)) {
		selectFloor_ = 4;
	}

	for (size_t i = 0; i < 5u; i++){
		if (isDraw_[i]) {
			isFloorDrawing_[i] = true;
		}
		else {
			isFloorDrawing_[i] = false;
		}
	}

	if (isAllDraw_)
		isFloorDrawing_ = { 0b11111 };

	map_->SetDraingFlag(isFloorDrawing_);

	obb_->Debug("OBB");
	obb_->Update();
}

void BlockEditor::Draw(const Camera& camera){
	obb_->Draw(camera.GetViewProjection());
}

void BlockEditor::Debug(){
	bool isChange = false;

#ifdef _DEBUG
	ImGui::Begin("ブロックエディター", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("ブロック整理")) {
			ImGui::Checkbox("全階層表示", &isAllDraw_);
			
			ImGui::Text("表示階層");
			for (size_t i = 0; i < 5u; i++){
				ImGui::Checkbox(std::to_string(i).c_str(), &isDraw_[i]);
				if (i == 4u)
					continue;
				ImGui::SameLine();
			}

			ImGui::Text("選択階層");

			for (size_t i = 0; i < 5u; i++){
				ImGui::RadioButton(("階層" + std::to_string(i)).c_str(), &selectFloor_, static_cast<int>(i));
				if (ImGui::IsItemActive()) {
					isFloorDrawing_[i] = true;
					isDraw_ = { 0,0,0,0,0 };
					isDraw_[i] = true;
				}				

				if (i == 4u)
					continue;
				ImGui::SameLine();
			}
			if (ImGui::Button("今の階層を前回のロード時までリセット")) {
				FloorReset();
			}
			if (ImGui::Button("前回のロード時までリセット")){
				DataReset();
			}
			ImGui::SameLine();

			if (ImGui::Button("全部リセット")) {
				AllDataReset();
			}

			for (size_t y = 0; y < 5u; y++) {
				if (selectFloor_ != y)
					continue;
				
				for (size_t z = 0; z < 10u; z++) {
					for (size_t x = 0; x < 10u; x++) {
						isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(z) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool&>((*mapSize_)[y][z][x]));
						if (ImGui::IsItemHovered()) {
							obb_->center_ = map_->GetGrobalPos(x, y, z);
							obb_->color_ = 0xff0000ff;
						}
						else {
								
						}
						if (x != 9) {
							ImGui::SameLine();
						}
					}
				}
					
			}


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("ファイル関連")) {
			// char型のバッファを用意して、std::stringを変換
			static char buffer[256]; // 適切なサイズに調整してください
			strcpy_s(buffer, sizeof(buffer), stageName_.c_str()); // std::stringをcharバッファにコピー

			// テキストボックスの作成
			ImGui::InputText("出力するファイル名", buffer, sizeof(buffer));

			stageName_ = buffer;

			if (ImGui::Button("選択したファイルに保存")) {
				if (OperationConfirmation()) {
					SaveFile(stageName_);
				}
			}
			if (ImGui::TreeNode("ファイル読み込み")) {				
				auto file = Lamb::GetFilePathFormDir(kDirectoryPath_, ".json");

				for (auto& i : file) {
					if (ImGui::Button(i.string().c_str())) {
						if (OperationConfirmation()) {
							LoadFiles(i.string());
						}
						break;
					}
				}
				
				ImGui::TreePop();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();

#endif // _DEBUG
}

void BlockEditor::DataReset(){
	if (OperationConfirmation()){
		*mapSize_ = beforeMapSize_;
	}
}

void BlockEditor::FloorReset(){
	if (OperationConfirmation()) {		
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
				((*mapSize_)[selectFloor_][z][x]) = beforeMapSize_[selectFloor_][z][x];
			}
		}		
	}
}

void BlockEditor::AllDataReset(){
	if (OperationConfirmation()) {
		(*mapSize_).fill({ {Map::BoxType::kNone} });
	}
}

bool BlockEditor::OperationConfirmation(){
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

void BlockEditor::SaveFile(const std::string& fileName){
	//保存
	json root;
	root = json::object();
	
	// 3次元配列をJSONオブジェクトに変換
	for (size_t y = 0; y < Map::kMapY; ++y) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
				root["boxes"][y][z][x] = static_cast<int>((*mapSize_)[y][z][x]);
			}
		}
	}

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
		return;
	}
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	std::string message = "File save completed.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
}

void BlockEditor::ChackFiles(){
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

void BlockEditor::LoadFiles(const std::string& fileName){
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

		if (filePath == fileName) {
			//ファイル読み込み
			LoadFile(fileName);
			return;
		}
	}
	std::string message = "File not found.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
}

void BlockEditor::LoadFile(const std::string& fileName){
	if (!LoadChackItem(fileName))
		return;
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = fileName;
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

	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName_);
	//未登録チェック
	assert(itGroup != root.end());

	

	//各アイテムについて
	for (size_t y = 0; y < Map::kMapY; ++y) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
				((*mapSize_)[y][z][x]) = root["boxes"][y][z][x];
			}
		}
	}

	beforeMapSize_ = *mapSize_;

#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

bool BlockEditor::LoadChackItem(const std::string& fileName){
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = fileName;
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

void BlockEditor::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
