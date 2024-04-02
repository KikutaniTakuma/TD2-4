#include "BlockEditor.h"
#include"fstream"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"

void BlockEditor::Initialize(){
	map_ = GameManager::GetInstance()->GetMap();

	mapSize_ = map_->GetBlockMap();

}

void BlockEditor::Finalize(){
}

void BlockEditor::Update(){
}

void BlockEditor::Draw(const Camera* camera){
	camera->fov;

}

void BlockEditor::Debug(){
}

void BlockEditor::AddItem(const Vector3& pos, const Vector3& scale){
	pos;
	scale;

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
	int i = 0;

	for (auto it = mapSize_[0].begin(); it != mapSize_[0].end(); ++it) {
		//　保存する先のコンテナ
		auto& item = root[kItemName_][i];
		item["Scale"] = json::array({
				it[0][0],
				it[0][0],
				it[0][0]
			});
		item["Rotate"] = json::array({
				it[0][0],
				it[0][0],
				it[0][0]
			});
		item["Pos"] = json::array({
				it[0][0],
				it[0][0],
				it[0][0]
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
		ofs << std::setw(4) << item << std::endl;
		//ファイルを閉じる
		ofs.close();
		i++;
	}

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

		if (filePath.stem().string() == fileName) {
			//ファイル読み込み
			LoadFile(filePath.stem().string());
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

bool BlockEditor::LoadChackItem(const std::string& fileName){
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

void BlockEditor::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
