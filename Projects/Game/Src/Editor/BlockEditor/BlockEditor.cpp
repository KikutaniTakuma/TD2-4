#include "BlockEditor.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"
#include"Input/Mouse/Mouse.h"
#include "Utils/UtilsLib/UtilsLib.h"

void BlockEditor::Initialize() {

	map_ = GameManager::GetInstance()->GetMap()->GetGroundEditor();

	mapSize_ = map_->GetGroundData();

	input_ = Input::GetInstance();
	for (size_t i = 0; i < 3; i++) {
		obb_[i] = std::make_unique<Obb>();

		obb_[i]->scale_ = { 1.f,1.f,3.2f };

		obb_[i]->color_ = 0x0000ffff;
	}



	reticle_ = std::make_unique<Obb>();

	reticle_->scale_ = { 1.f,1.f,1.f };

	reticle_->color_ = 0xff0000ff;

	scanningOBB_ = std::make_unique<Obb>();

	scanningOBB_->scale_ = { 1.f,1.f,1.f };


}

void BlockEditor::Finalize() {

}

void BlockEditor::Update() {


	if (obb_[1]->OBBinPoint(reticle_->center_)) {
		reticle_->color_ = 0x00ff00ff;
	}
	else {
		reticle_->color_ = 0x000000ff;
	}

	if (MapinPoint(reticle_->center_))
		isUseImgui_ = true;

	for (size_t i = 0; i < 3; i++) {
		obb_[i]->Update();
	}

	reticle_->Update();
	scanningOBB_->Update();
}

void BlockEditor::Draw(const Camera &camera) {
	for (size_t i = 0; i < 3; i++) {
		obb_[i]->Draw(camera.GetViewProjection());
	}
}

void BlockEditor::Debug() {
	bool isChange = false;

#ifdef _DEBUG
	if (isUseImgui_) {

		ImGui::Begin("ブロックエディター", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("ブロック整理")) {

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

				for (size_t x = 0; x < GroundEditor::kMapX; x++) {
					isChange |= ImGui::Checkbox(("##Checkbox" + std::to_string(0) + ' ' + std::to_string(x)).c_str(), &reinterpret_cast<bool&>((*mapSize_)[x].isMultiSelect_));
					if (ImGui::IsItemHovered()) {
						for (size_t i = 0; i < 3; i++) {
							obb_[i]->center_ = map_->GetGrobalPos((int32_t)(x - 1 + i), -1, 0);
							obb_[i]->color_ = 0xff0000ff;
						}

					}
					else {

					}
					if (x != 9) {
						ImGui::SameLine();
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

					for (auto &i : file) {
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
				ImGui::DragFloat("どれだけ離すか", &correction_, 0.1f, 0.0f, 15.0f);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}
	if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (Mouse::GetInstance()->Pushed(Mouse::Button::Left)) {
			(*mapSize_)[boxPos_[0] - 1].isConstruction = true;
			(*mapSize_)[boxPos_[0]].isConstruction = true;
			(*mapSize_)[boxPos_[0] + 1].isConstruction = true;
		}
		else if (Mouse::GetInstance()->Pushed(Mouse::Button::Right)) {
			(*mapSize_)[boxPos_[0] - 1].isConstruction = false;
			(*mapSize_)[boxPos_[0]].isConstruction = false;
			(*mapSize_)[boxPos_[0] + 1].isConstruction = false;
		}


	}


#endif // _DEBUG
}

void BlockEditor::DataReset() {
	/*if (OperationConfirmation()) {
		*houseList_ = beforeHouseList_;
	}*/
}

void BlockEditor::FloorReset() {
	if (OperationConfirmation()) {
		//for (size_t z = 0; z < Map::kMapZ; ++z) {
		for (size_t x = 0; x < Map::kMapX; ++x) {
		}
		//}		
	}
}

void BlockEditor::AllDataReset() {
	if (OperationConfirmation()) {

	}
}

bool BlockEditor::OperationConfirmation() {
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

bool BlockEditor::MapinPoint(const Vector3 &point) {
	//for (size_t z = 0; z < Map::kMapZ; z++) {
	for (int32_t x = 0; x < GroundEditor::kMapX; x++) {
		if (x == 0 || x == 29)
			continue;
		scanningOBB_->center_ = map_->GetGrobalPos(static_cast<int32_t>(x), static_cast<int32_t>(-1), 0);
		scanningOBB_->center_.z -= correction_;
		if (scanningOBB_->OBBinPoint(point)) {
			for (int32_t i = 0; i < 3; i++) {
				obb_[i]->center_ = map_->GetGrobalPos(x - 1 + i, -1, 0);
				//obb_[i]->center_.z += correction_;
			}
			boxPos_ = { static_cast<size_t>(x),static_cast<size_t>(-1) };
			return true;
		}
	}
	//}

	return false;
}

void BlockEditor::MousePosTrans(const Camera &camera) {
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
	float distancePlayerTo3DReticle = -camera.pos.z - correction_;
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

void BlockEditor::SaveFile(const std::string &fileName) {
	//保存
	json root;
	root = json::object();

	 //3次元配列をJSONオブジェクトに変換
	
	for (size_t x = 0; x < Map::kMapX; ++x) {
		root["boxes"][x] = ((*mapSize_)[x].isConstruction);
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

void BlockEditor::ChackFiles() {
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry &entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

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

void BlockEditor::LoadFiles(const std::string &fileName) {
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::string message = "This file path does not exist.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry &entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

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
	std::wstring message = L"ファイルが見つかりませんでした。代わりにステージ1を読み込みます";
	MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"ブロックないよぉ！", 0);
	LoadFile("Resources/Datas/Boxes/stage1.json");
}

void BlockEditor::LoadFiles(const int32_t selectNum) {
	std::string fileName = (kDirectoryPath_ + "stage" + std::to_string(selectNum + 1) + ".json").c_str();

	if (!std::filesystem::exists(kDirectoryName_)) {
		std::string message = "This file path does not exist.";
		MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry &entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

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
	std::wstring message = L"ファイルが見つかりませんでした。代わりにステージ1を読み込みます";
	MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"ブロックないよぉ！", 0);
	LoadFile("Resources/Datas/Boxes/stage1.json");
}

void BlockEditor::LoadFile(const std::string &fileName) {
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
	for (size_t x = 0; x < GroundEditor::kMapX; ++x) {
		((*mapSize_)[x].isConstruction) = root["boxes"][x];
	}

#ifdef _DEBUG

	// コンパイル時に文字列生成してみた
	using Message = SoLib::Text::StaticString<"File loading completed"> ;
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), Message::c_str(), "Object", 0);

#endif // _DEBUG
}

bool BlockEditor::LoadChackItem(const std::string &fileName) {
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

void BlockEditor::from_json(const json &j, Vector3 &v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
