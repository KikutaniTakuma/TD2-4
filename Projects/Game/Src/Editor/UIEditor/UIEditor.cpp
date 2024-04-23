#include "UIEditor.h"
#include"imgui.h"
#include"Drawers/DrawerManager.h"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"
#include"Input/Mouse/Mouse.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <fstream>

UIEditor::~UIEditor(){

}

void UIEditor::Initialize(class SceneManager* sceneManager){
	sceneManager_ = sceneManager;

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

#ifdef _DEBUG
	sceneName_[BaseScene::ID::Title] = "Title";
	sceneName_[BaseScene::ID::Game] = "Game";
	sceneName_[BaseScene::ID::StageSelect] = "Select";
	sceneName_[BaseScene::ID::Result] = "Result";
#endif // _DEBUG

}

void UIEditor::Finalize(){

}

void UIEditor::Update(){
	Debug();

	for (size_t i = 0; i < BaseScene::maxScene_; i++){
		for (size_t j = 0; j < texies_[i].size(); j++){
			texies_[i][j]->transform.CalcMatrix();
		}
	}


}

void UIEditor::Draw(const Camera& camera){	

	for (size_t i = 0; i < BaseScene::maxScene_; i++) {
		for (size_t j = 0; j < texies_[i].size(); j++) {
			tex2D_->Draw(texies_[i][j]->transform.matWorld_, Mat4x4::kIdentity, camera.GetViewOthographics()
				, texies_[i][j]->textureID, texies_[i][j]->color, BlendType::kNormal);
		}
	}
}

void UIEditor::Debug(){

#ifdef _DEBUG
	ImGui::Begin("UIエディター", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("UI生成")) {
			auto file = Lamb::GetFilePathFormDir("Resources/", ".png");
			for (auto& i : file) {
				if (ImGui::Button(i.string().c_str())) {
					
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("ファイル関連")) {
			for (auto& i : sceneName_) {
				if (ImGui::Button(i.second.c_str())) {
					stageName_ = i.second.c_str();
					break;
				}
			}

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

void UIEditor::LoadFiles(const std::string& fileName){
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
	std::wstring message = L"ファイルが見つかりませんでした。";
	MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"UIないよぉ！", 0);
}

void UIEditor::SaveFile(const std::string& fileName){
	//保存
	json root;
	root = json::object();
	root[kItemName_] = json::object();
	/*保存するものを書き込む*/


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

void UIEditor::ChackFiles(){
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

void UIEditor::LoadFile(const std::string& fileName){
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

	

	////グループを検索
	//nlohmann::json::iterator itGroupNum = root[kItemName_].find("Num");
	////未登録チェック
	//assert(itGroupNum != root[kItemName_].end());

	//

	////グループを検索
	//nlohmann::json::iterator itGroupPos = root[kItemName_].find("Pos");
	////未登録チェック
	//assert(itGroupPos != root[kItemName_].end());

#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

bool UIEditor::LoadChackItem(const std::string& fileName){
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

bool UIEditor::OperationConfirmation(){
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

void UIEditor::from_json(const json& j, Vector2& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
}
