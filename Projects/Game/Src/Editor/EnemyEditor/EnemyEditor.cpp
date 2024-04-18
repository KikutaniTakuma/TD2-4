#include "EnemyEditor.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"
#include"Input/Mouse/Mouse.h"

void EnemyEditor::Initialize(){
	map_ = GameManager::GetInstance()->GetMap();

	mapSize_ = map_->GetBlockMap();

	beforeMapSize_ = *mapSize_;

	input_ = Input::GetInstance();

	obb_ = std::make_unique<Obb>();

	obb_->scale_ = { 1.f,1.f,3.2f };

	obb_->color_ = 0x0000ffff;

	reticle_ = std::make_unique<Obb>();

	reticle_->scale_ = { 1.f,1.f,1.f };

	reticle_->color_ = 0xff0000ff;

	scanningOBB_ = std::make_unique<Obb>();

	scanningOBB_->scale_ = { 1.f,1.f,1.f };
}

void EnemyEditor::Finalize(){

}

void EnemyEditor::Update(){
	if (obb_->OBBinPoint(reticle_->center_)) {
		reticle_->color_ = 0x00ff00ff;
	}
	else {
		reticle_->color_ = 0x000000ff;
	}

	MapinPoint(reticle_->center_);

	obb_->Update();

	reticle_->Update();
	scanningOBB_->Update();
}

void EnemyEditor::Draw(const Camera& camera){
	obb_->Draw(camera.GetViewProjection());
}

void EnemyEditor::Debug(){
	bool isChange = false;

#ifdef _DEBUG
	ImGui::Begin("エネミーエディター", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("エネミー整理")) {

			ImGui::Text("表示階層");

			ImGui::Text("選択階層");

			for (size_t i = 0; i < 5u; i++) {

				if (i == 4u)
					continue;
				ImGui::SameLine();
			}
			if (ImGui::Button("今の階層を前回のロード時までリセット")) {
				FloorReset();
			}
			if (ImGui::Button("前回のロード時までリセット")) {
				DataReset();
			}
			ImGui::SameLine();

			if (ImGui::Button("全部リセット")) {
				AllDataReset();
			}

			for (size_t y = 0; y < Map::kMapY; y++) {

				for (size_t z = 0; z < Map::kMapZ; z++) {
					for (size_t x = 0; x < Map::kMapX; x++) {
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
		if (ImGui::BeginMenu("レティクルテスト")) {
			ImGui::DragFloat("レティクルとの距離", &distancePlayerTo3DReticleCopy_, 0.1f, 0.0f, 30.0f);
			ImGui::DragFloat3("レティクルのポジション", &reticle_->center_.x, 0.1f);
			//ImGui::DragFloat("どれだけ離すか", &correction_, 0.1f, 0.0f, 15.0f);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();
	if (input_->GetKey()->LongPush(DIK_LSHIFT)){
		if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			bool& num = (*mapSize_)[boxPos_[1]][boxPos_[2]][boxPos_[0]].isMultiSelect_;
			if (Mouse::GetInstance()->Pushed(Mouse::Button::Left)) {
				multiMode_ = true;
				num = true;
			}
			else if (Mouse::GetInstance()->Pushed(Mouse::Button::Right)) {
				num = false;
			}
		}
	}
	else {
		if (multiMode_){
			if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {			

				if (Mouse::GetInstance()->Pushed(Mouse::Button::Left)) {

					bool select = (*mapSize_)[boxPos_[1]][boxPos_[2]][boxPos_[0]].isMultiSelect_;

					if (select){

						for (size_t x = 0; x < Map::kMapX; x++) {
							Map::BoxInfo& num = (*mapSize_)[boxPos_[1]][boxPos_[2]][x];

							if (num.isMultiSelect_) {

								if (num.dwarfNum < 2u) {
									if (num.dwarfNum > 0) {
										setPos_ = map_->GetGrobalPos(x, 2, 0);
										setPos_.y -= 0.5f;
									}
									else {
										setPos_ = map_->GetGrobalPos(x, 1, 0);
									}
									setPos_.z -= 5.0;

									EnemyManager::GetInstance()->AddEnemy(setPos_);

									num.dwarfNum++;
								}								
							}
						}						
					}					
				}
				else if(Mouse::GetInstance()->Pushed(Mouse::Button::Right)) {
					int32_t& num = (*mapSize_)[boxPos_[1]][boxPos_[2]][boxPos_[0]].dwarfNum;
					if (num > 0) {
						EnemyManager::GetInstance()->DeadEnemy(delPos_);
						num--;
					}
				}
			}
			if (input_->GetKey()->Pushed(DIK_R)){
				multiMode_ = false;
				map_->MultiReset();
			}
		}
		else {
			if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				int32_t& num = (*mapSize_)[boxPos_[1]][boxPos_[2]][boxPos_[0]].dwarfNum;
				if (Mouse::GetInstance()->Pushed(Mouse::Button::Left)) {
					if (num < 2u) {
						EnemyManager::GetInstance()->AddEnemy(setPos_);
						num++;
					}
				}
				else if (Mouse::GetInstance()->Pushed(Mouse::Button::Right)) {
					if (num > 0) {
						EnemyManager::GetInstance()->DeadEnemy(delPos_);
						num--;
					}
				}
			}
		}
	}

#endif // _DEBUG
}

void EnemyEditor::DataReset(){
	if (OperationConfirmation()) {
		*mapSize_ = beforeMapSize_;
	}
}

void EnemyEditor::FloorReset(){
	if (OperationConfirmation()) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
			}
		}
	}
}

void EnemyEditor::AllDataReset(){
	if (OperationConfirmation()) {

	}
}

bool EnemyEditor::OperationConfirmation(){
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

bool EnemyEditor::MapinPoint(const Vector3& point){
	for (size_t y = 0; y < Map::kMapY; y++) {
		for (size_t z = 0; z < Map::kMapZ; z++) {
			for (size_t x = 0; x < Map::kMapX; x++) {
				int32_t num = (*mapSize_)[boxPos_[1]][boxPos_[2]][boxPos_[0]].dwarfNum;
				scanningOBB_->center_ = map_->GetGrobalPos(x, y, z);
				scanningOBB_->center_.z -= correction_;
				if (scanningOBB_->OBBinPoint(point)) {
					obb_->center_ = map_->GetGrobalPos(x, y, z);
					if (num > 0) {
						setPos_ = map_->GetGrobalPos(x, y + 2, z);
						setPos_.y -= 0.5f;
					}
					else {
						setPos_ = map_->GetGrobalPos(x, y + 1, z);
					}
					if (num > 1) {
						delPos_ = map_->GetGrobalPos(x, y + 2, z);
						delPos_.y -= 0.5f;
					}
					else {
						delPos_ = map_->GetGrobalPos(x, y + 1, z);
					}
					setPos_.z -= 5.0;
					delPos_.z -= 5.0f;
					//obb_[i]->center_.z += correction_;
					boxPos_ = { x,y,z };
					boxVector_ = Vector3(float(boxPos_[0]), float(boxPos_[1]), float(boxPos_[2]));
					return true;
				}
			}
		}

	}
	return false;
}

void EnemyEditor::MousePosTrans(const Camera& camera){
	Vector2 mousePou = Mouse::GetInstance()->GetPos();
	Mat4x4 matVPV = camera.GetViewProjectionVp();
	Mat4x4 matInverseVPV = matVPV.Inverse();

	Vector3 posNear = Vector3(mousePou.x, mousePou.y, 0);
	Vector3 posFar = Vector3(mousePou.x, mousePou.y, 1);

	posNear = posNear * matInverseVPV;
	posFar = posFar * matInverseVPV;

	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = mouseDirection.Normalize();

	// 自機から3Dレティクルへの距離
	float distancePlayerTo3DReticle = 40.0f - correction_;
	float minRange = (-distancePlayerTo3DReticle - 0.05f);
	float maxRange = (-distancePlayerTo3DReticle + 0.05f);
	float chackNum = 0;

	while (camera.pos.z - (posNear + mouseDirection * distancePlayerTo3DReticle).z < minRange) {
		chackNum = camera.pos.z - (posNear + mouseDirection * distancePlayerTo3DReticle).z;
		distancePlayerTo3DReticle -= 0.01f;
	}
	while (camera.pos.z - (posNear + mouseDirection * distancePlayerTo3DReticle).z > maxRange) {
		chackNum = camera.pos.z - (posNear + mouseDirection * distancePlayerTo3DReticle).z;
		distancePlayerTo3DReticle += 0.01f;
	}
	distancePlayerTo3DReticleCopy_ = distancePlayerTo3DReticle;
	reticle_->center_ = posNear + mouseDirection * distancePlayerTo3DReticle;
}

void EnemyEditor::SaveFile(const std::string& fileName){
	//保存
	json root;
	root = json::object();
	root[kItemName_] = json::object();

	// 3次元配列をJSONオブジェクトに変換
	for (size_t y = 0; y < Map::kMapY; ++y) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
				root[kItemName_]["Num"][x] = static_cast<int>((*mapSize_)[y][z][x].dwarfNum);
			}
		}
	}

	for (size_t y = 0; y < Map::kMapY; ++y) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < EnemyManager::GetInstance()->GetStartPosListSize(); ++x) {
				root[kItemName_]["Pos"][x] =json::array({
					   EnemyManager::GetInstance()->GetEnemyStartPos(x).x,
					   EnemyManager::GetInstance()->GetEnemyStartPos(x).y,
					   EnemyManager::GetInstance()->GetEnemyStartPos(x).z
				});				
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

void EnemyEditor::ChackFiles(){
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

void EnemyEditor::LoadFiles(const std::string& fileName){
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

void EnemyEditor::LoadFile(const std::string& fileName){
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
	nlohmann::json::iterator itGroupEn = root.find(kItemName_);
	//未登録チェック
	assert(itGroupEn != root.end());

	EnemyManager::GetInstance()->ListReset();
	
	//グループを検索
	nlohmann::json::iterator itGroupNum = root[kItemName_].find("Num");
	//未登録チェック
	assert(itGroupNum != root[kItemName_].end());

	//各アイテムについて
	for (size_t y = 0; y < Map::kMapY; ++y) {
		for (size_t z = 0; z < Map::kMapZ; ++z) {
			for (size_t x = 0; x < Map::kMapX; ++x) {
				((*mapSize_)[y][z][x].dwarfNum) = root[kItemName_]["Num"][x];
			}
		}
	}

	//グループを検索
	nlohmann::json::iterator itGroupPos = root[kItemName_].find("Pos");
	//未登録チェック
	assert(itGroupPos != root[kItemName_].end());

	

	for (const auto& i : root[kItemName_]["Pos"]) {
		Vector3 newPos{};

		from_json(i, newPos);

		EnemyManager::GetInstance()->AddEnemy(newPos);
	}

	/*//グループを検索
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
	}*/


	beforeMapSize_ = *mapSize_;

#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG

}

bool EnemyEditor::LoadChackItem(const std::string& fileName){
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

void EnemyEditor::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
