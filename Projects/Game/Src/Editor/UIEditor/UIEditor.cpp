#include "UIEditor.h"
#include"imgui.h"
#include"Drawers/DrawerManager.h"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include"../SoLib/SoLib/SoLib_Json.h"
#include"Input/Mouse/Mouse.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <fstream>
#include <Utils/EngineInfo/EngineInfo.h>
#include"Error/Error.h"

UIEditor::~UIEditor(){

}

const float UIEditor::scaleMoveSpeed = 0.075f;

void UIEditor::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	sceneName_[BaseScene::ID::Title] = "Title";
	sceneName_[BaseScene::ID::Game] = "Game";
	sceneName_[BaseScene::ID::StageSelect] = "Select";
	sceneName_[BaseScene::ID::Result] = "Result";

	newTex_ = std::make_unique<Tex2DState>();
	newTex_->transform.translate = Vector2(0, 0);
	newTex_->transform.translate.y *= -1;
	newTex_->transform.translate += Vector2(-640, 360);
	newTex_->transform.scale = { 64,64 };
	newTex_->color = 0x000000ff;
	newTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/white2x2.png");
	
	input_ = Input::GetInstance();

	easing_ = std::make_unique<Easeing>();
	//texAnim_ = std::make_unique<Tex2DAniamtor>();

	//texAnim_->SetStartPos(Vector2::kZero);
	//texAnim_->SetDuration(0.1f);
	//texAnim_->SetAnimationNumber(6);
	//texAnim_->SetLoopAnimation(true);
	//texAnim_->Start();
	//animNum_ = DrawerManager::GetInstance()->LoadTexture("./Resources/Load.png");

	potGoalScale_ = { 84.0f,44.0f };
	potBaseScale_ = { 64.0f,64.0f };

	camera_.Identity();
}

void UIEditor::Finalize(){

}

void UIEditor::Update(const BaseScene::ID id){
	id_ = id;
	Debug(id);
	if (Mouse::GetInstance()->Pushed(Mouse::Button::Right)) {
		newTex_->transform.translate = Mouse::GetInstance()->GetPos();
		newTex_->transform.translate.y *= -1;
		newTex_->transform.translate += Vector2{ -640, 360 };
	}

	
	newTex_->transform.CalcMatrix();
	easing_->Update();

	if (!isScaleMoveReverse_ and easing_->ActiveExit()) {
		isScaleMoveReverse_ = true;
		easing_->Start(false, time_, Easeing::InSine);
	}
	else if (isScaleMoveReverse_ and easing_->ActiveExit()) {
		isScaleMoveReverse_ = false;
		isScaleMove_ = false;
	}

	for (size_t i = 0; i < BaseScene::kMaxScene; i++){
		if (i != static_cast<size_t>(id))
			continue;

		for (size_t j = 0; j < texies_[i].size(); j++){
			if (texies_[i][j]->textureName == "backGround") {
				Vector3& uvTranslate = texies_[i][j]->uvTransform.translate;

				uvTranslate.x += 0.001f;
				uvTranslate.y += 0.0005f;
				if (1.0f <= uvTranslate.x) {
					uvTranslate.x -= 1.0f;
				}
				if (1.0f <= uvTranslate.y) {
					uvTranslate.y -= 1.0f;
				}
			}

			GameControlUIMove(i, j);
			PotScaleMove(i, j);

			texies_[i][j]->transform.CalcMatrix();
			texies_[i][j]->uvTransform.CalcMatrix();
		}
	}


}

void UIEditor::Draw(const Mat4x4& camera, const BaseScene::ID id){
	camera_ = camera;
	//texAnim_->Update();
	//tex2D_->Draw(
	//	Mat4x4::MakeAffin(Vector3(Lamb::ClientSize(), 1.0f), Vector3::kZero, Vector3::kZero),
	//	texAnim_->GetUvMat4x4(),
	//	camera,
	//	animNum_,
	//	std::numeric_limits<uint32_t>::max(),
	//	BlendType::kNone
	//);

	for (size_t i = 0; i < BaseScene::kMaxScene; i++) {
		if (i != static_cast<size_t>(id))
			continue;
		for (size_t j = 0; j < texies_[i].size(); j++) {
			tex2D_->Draw(texies_[i][j]->transform.matWorld_, texies_[i][j]->uvTransform.matWorld_, camera
				, texies_[i][j]->textureID, texies_[i][j]->color, BlendType::kNormal);
		}
	}
}

void UIEditor::PutDraw(const Mat4x4& camera){
	tex2D_->Draw(newTex_->transform.matWorld_, Mat4x4::kIdentity, camera
		, newTex_->textureID, newTex_->color, BlendType::kNormal);
}

void UIEditor::Debug(const BaseScene::ID id){
	id;
#ifdef _DEBUG
	ImGui::Begin("UIエディター", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("UI生成")) {
			ImGui::DragFloat2("生成するポジション", &newTex_->transform.translate.x, 1.0f);
			if (ImGui::TreeNode("生成するUI画像")) {
				auto file = Lamb::GetFilePathFormDir("Resources/UI/", ".png");
				for (auto& i : file) {
					if (ImGui::Button(i.string().c_str())) {						
						if (OperationConfirmation()) {
							std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
							setTex_->color = 0xffffffff;
							setTex_->transform = newTex_->transform;
							setTex_->textureID = DrawerManager::GetInstance()->LoadTexture(i.string().c_str());

							setTex_->textureFullPath = i.string().c_str();
							std::string result;

							size_t slashPos_ = i.string().find_last_of('/');
							size_t lSlashPos_ = i.string().find_last_of('\\');
							size_t dotPos_ = i.string().find_last_of('.');
							if (lSlashPos_ > slashPos_) {
								if (lSlashPos_ != std::string::npos && dotPos_ != std::string::npos && dotPos_ > lSlashPos_) {
									result = i.string().substr(lSlashPos_ + 1, dotPos_ - lSlashPos_ - 1);
								}
							}
							else {
								if (slashPos_ != std::string::npos && dotPos_ != std::string::npos && dotPos_ > slashPos_) {
									result = i.string().substr(slashPos_ + 1, dotPos_ - slashPos_ - 1);
								}
							}
							setTex_->textureName = result;
							texies_[static_cast<size_t>(id)].emplace_back(std::move(setTex_));
						}


						break;
					}
				}
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("配置しているUI")) {
			for (size_t i = 0; i < texies_[static_cast<size_t>(id)].size(); i++){
				if (ImGui::TreeNode((texies_[static_cast<size_t>(id)][i]->textureName.c_str() + std::to_string(i)).c_str())) {
					ImGui::DragFloat3("ポジション", texies_[static_cast<size_t>(id)][i]->transform.translate.data(), 1.0f);
					ImGui::DragFloat2("大きさ", texies_[static_cast<size_t>(id)][i]->transform.scale.data(), 1.0f);
					ImGui::DragFloat2("UVポジション", texies_[static_cast<size_t>(id)][i]->uvTransform.translate.data(), 1.0f);
					ImGui::DragFloat2("UV大きさ",texies_[static_cast<size_t>(id)][i]->uvTransform.scale.data(), 1.0f);
					uint32_t colorInt = texies_[static_cast<size_t>(id)][i]->color;					
					Vector4 color = colorInt;
					 ImGui::ColorEdit4("テクスチャの色", &color.vec.x, true);
					 texies_[static_cast<size_t>(id)][i]->color = color.GetColorRGBA();
					 if (ImGui::Button("このテクスチャを削除する")){
						 if (OperationConfirmation()) {
							 texies_[static_cast<size_t>(id)].erase(texies_[static_cast<size_t>(id)].begin() + i);
							 i--;
						 }
					 }
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("ファイル関連")) {
			ImGui::Text("選択したファイルを保存");
			for (auto& i : sceneName_) {
				if (ImGui::Button(i.second.c_str())) {
					stageName_ = i.second.c_str();
					if (OperationConfirmation()) {
						SaveFile(i.second.c_str());
					}
					break;
				}
			}
			if (ImGui::Button("全てのシーンを保存する")){
				if (OperationConfirmation()) {
					for (auto& i : sceneName_) {
						stageName_ = i.second.c_str();
						SaveFile(i.second.c_str());
					}
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
				if (ImGui::Button("全てのシーンのUIを読み込む")) {
					if (OperationConfirmation()) {
						LoadFileAll();
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
		throw Lamb::Error::Code<UIEditor>(message, ErrorPlace);
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

void UIEditor::GameControlUIMove(const size_t i, const size_t j){
	const auto& key = input_->GetKey();

	auto& tex = texies_[i][j];

	if (tex->textureName == "attackButton") {
		if (key->LongPush(DIK_SPACE)) {
			tex->uvTransform.translate.x = 0.5f;
		}
		else {
			tex->uvTransform.translate.x = 0.0f;
		}
	}

	if (tex->textureName == "carryButton") {
		if (key->LongPush(DIK_X)) {
			tex->uvTransform.translate.x = 0.5f;
		}
		else {
			tex->uvTransform.translate.x = 0.0f;
		}
	}

	if (tex->textureName == "moveButton") {
		if (tex->uvTransform.scale.x > 0) {
			if (key->GetKey(DIK_A) or key->GetKey(DIK_LEFT)) {
				tex->uvTransform.translate.x = 0.5f;
			}
			else {
				tex->uvTransform.translate.x = 0.0f;
			}
		}
		else {
			if (key->GetKey(DIK_D) or key->GetKey(DIK_RIGHT)) {
				tex->uvTransform.translate.x = 0.0f;
			}
			else {
				tex->uvTransform.translate.x = 0.5f;
			}
		}
	}

	if (tex->textureName == "jumpButton") {
		if (key->LongPush(DIK_Z)) {
			tex->uvTransform.translate.x = 0.5f;
		}
		else {
			tex->uvTransform.translate.x = 0.0f;
		}
	}

}

void UIEditor::PotScaleMove(const size_t i, const size_t j){
	auto& texScale = texies_[i][j]->transform.scale;

	texScale = potBaseScale_;

	if (!isScaleMove_)
	return;
	
	
	auto& texName = texies_[i][j]->textureName;
	
	if (texName == "pot") {	
		if (!isScaleMoveReverse_){
			texScale = easing_->Get(potBaseScale_, potGoalScale_);
		}
		else {
			texScale = easing_->Get(potGoalScale_, potBaseScale_);
		}
	}

}

void UIEditor::BeginScaleMove(const float time){
	if (isScaleMove_){
		easing_->Start(false, time, Easeing::InSine);
		time_ = time;
		isScaleMoveReverse_ = false;
	}
	else {
		isScaleMove_ = true;
		time_ = time;
		easing_->Start(false, time, Easeing::InSine);
		isScaleMoveReverse_ = false;
	}
}

void UIEditor::SaveFile(const std::string& fileName){
	//保存
	json root;
	root = json::object();
	root[kItemName_] = json::object();
	root[kItemName_][fileName];
	/*
	* 座標
	* 大きさ
	* 色
	* テクスチャの名前
	* テクスチャのフルパス
	* 
	*/
	
	/*保存するものを書き込む*/
	if (sceneName_[BaseScene::ID::Title] == fileName){
		for (size_t i = 0; i < texies_[1].size(); i++) {
		root[kItemName_]["Title"][i]["translate"] = json::array({
			   texies_[1][i]->transform.translate.x,
			   texies_[1][i]->transform.translate.y,
			   texies_[1][i]->transform.translate.z
			});
		root[kItemName_]["Title"][i]["scale"] = json::array({
			   texies_[1][i]->transform.scale.x,
			   texies_[1][i]->transform.scale.y,
			   texies_[1][i]->transform.scale.z
			});
		root[kItemName_]["Title"][i]["UVTranslate"] = json::array({
			   texies_[1][i]->uvTransform.translate.x,
			   texies_[1][i]->uvTransform.translate.y
			});
		root[kItemName_]["Title"][i]["UVScale"] = json::array({
			   texies_[1][i]->uvTransform.scale.x,
			   texies_[1][i]->uvTransform.scale.y
			});
			root[kItemName_]["Title"][i]["color"] = texies_[1][i]->color;
			root[kItemName_]["Title"][i]["TextureName"] = texies_[1][i]->textureName;
			root[kItemName_]["Title"][i]["TextureFullPath"] = texies_[1][i]->textureFullPath;
		}

	}
	else if (sceneName_[BaseScene::ID::Game] == fileName){
		for (size_t i = 0; i < texies_[3].size(); i++) {
			root[kItemName_]["Game"][i]["translate"] = json::array({
				   texies_[3][i]->transform.translate.x,
				   texies_[3][i]->transform.translate.y,
				   texies_[3][i]->transform.translate.z
				});
			root[kItemName_]["Game"][i]["scale"] = json::array({
				   texies_[3][i]->transform.scale.x,
				   texies_[3][i]->transform.scale.y,
				   texies_[3][i]->transform.scale.z
				});
			root[kItemName_]["Game"][i]["UVTranslate"] = json::array({
				   texies_[3][i]->uvTransform.translate.x,
				   texies_[3][i]->uvTransform.translate.y
				});
			root[kItemName_]["Game"][i]["UVScale"] = json::array({
				   texies_[3][i]->uvTransform.scale.x,
				   texies_[3][i]->uvTransform.scale.y
				});
			root[kItemName_]["Game"][i]["color"] = texies_[3][i]->color;
			root[kItemName_]["Game"][i]["TextureName"] = texies_[3][i]->textureName;
			root[kItemName_]["Game"][i]["TextureFullPath"] = texies_[3][i]->textureFullPath;
		}

	}
	else if (sceneName_[BaseScene::ID::StageSelect] == fileName){
		for (size_t i = 0; i < texies_[2].size(); i++) {
			root[kItemName_]["Select"][i]["translate"] = json::array({
				   texies_[2][i]->transform.translate.x,
				   texies_[2][i]->transform.translate.y,
				   texies_[2][i]->transform.translate.z
				});
			root[kItemName_]["Select"][i]["scale"] = json::array({
				   texies_[2][i]->transform.scale.x,
				   texies_[2][i]->transform.scale.y,
				   texies_[2][i]->transform.scale.z
				});
			root[kItemName_]["Select"][i]["UVTranslate"] = json::array({
				   texies_[2][i]->uvTransform.translate.x,
				   texies_[2][i]->uvTransform.translate.y
				});
			root[kItemName_]["Select"][i]["UVScale"] = json::array({
				   texies_[2][i]->uvTransform.scale.x,
				   texies_[2][i]->uvTransform.scale.y
				});
			root[kItemName_]["Select"][i]["color"] = texies_[2][i]->color;
			root[kItemName_]["Select"][i]["TextureName"] = texies_[2][i]->textureName;
			root[kItemName_]["Select"][i]["TextureFullPath"] = texies_[2][i]->textureFullPath;
		}

	}
	else if (sceneName_[BaseScene::ID::Result] == fileName){
		for (size_t i = 0; i < texies_[0].size(); i++) {
			root[kItemName_]["Result"][i]["translate"] = json::array({
				   texies_[0][i]->transform.translate.x,
				   texies_[0][i]->transform.translate.y,
				   texies_[0][i]->transform.translate.z
				});
			root[kItemName_]["Result"][i]["scale"] = json::array({
				   texies_[0][i]->transform.scale.x,
				   texies_[0][i]->transform.scale.y,
				   texies_[0][i]->transform.scale.z
				});
			root[kItemName_]["Result"][i]["UVTranslate"] = json::array({
				   texies_[0][i]->uvTransform.translate.x,
				   texies_[0][i]->uvTransform.translate.y
				});
			root[kItemName_]["Result"][i]["UVScale"] = json::array({
				   texies_[0][i]->uvTransform.scale.x,
				   texies_[0][i]->uvTransform.scale.y
				});
			root[kItemName_]["Result"][i]["color"] = texies_[0][i]->color;
			root[kItemName_]["Result"][i]["TextureName"] = texies_[0][i]->textureName;
			root[kItemName_]["Result"][i]["TextureFullPath"] = texies_[0][i]->textureFullPath;

		}

	}
	else {
		std::wstring message = L"この名前のシーンが見つかりませんでした。";
		MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"シーンないよぉ！", 0);
		return;
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
		throw Lamb::Error::Code<UIEditor>(message, ErrorPlace);
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
		throw Lamb::Error::Code<UIEditor>(message, ErrorPlace);
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
		throw Lamb::Error::Code<UIEditor>(message, ErrorPlace);
	}

	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	nlohmann::json::iterator itGroupUI = root.find(kItemName_);
	//未登録チェック
	assert(itGroupUI != root.end());
	

	//グループを検索
	nlohmann::json::iterator itGroupNum = root[kItemName_].find(sceneName_[id_].c_str());
	//未登録チェック
	if (itGroupNum == root[kItemName_].end()) {
		std::wstring message = L"選択したシーンと一致しませんでした。";
		MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"シーンが合ってないよぉ！", 0);
		return;
	}

	texies_[static_cast<size_t>(id_)].clear();

	for (const auto& i : root[kItemName_][sceneName_[id_].c_str()]) {
		std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
		Vector3 pos, scale;
		Vector2 UVpos, UVscale;
		from_json(i["scale"], scale);
		from_json(i["translate"], pos);
		from_json(i["UVScale"], UVscale);
		from_json(i["UVTranslate"], UVpos);
		setTex_->color = i["color"];
		setTex_->transform.scale = scale;
		setTex_->transform.translate = pos;
		setTex_->uvTransform.scale = UVscale;
		setTex_->uvTransform.translate = UVpos;
		setTex_->textureID = DrawerManager::GetInstance()->LoadTexture(i["TextureFullPath"]);
		setTex_->textureFullPath = i["TextureFullPath"];
		setTex_->textureName = i["TextureName"];
		

		texies_[static_cast<size_t>(id_)].emplace_back(std::move(setTex_));

	}

	

	

#ifdef _DEBUG
	std::string message = "File loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

#endif // _DEBUG
}

void UIEditor::LoadFileAll(){
	for (auto& scene : sceneName_) {
		std::string file = (kDirectoryPath_ + scene.second + ".json");
		if (!LoadChackItem(file))
			continue;
		//読み込むjsonファイルのフルパスを合成する
		std::string filePath = file;
		//読み込み用のファイルストリーム
		std::ifstream ifs;
		//ファイルを読み込み用に開く
		ifs.open(filePath);
		// ファイルオープン失敗
		if (ifs.fail()) {
			std::string message = "Failed open data file for write.";
			throw Lamb::Error::Code<UIEditor>(message, ErrorPlace);
		}

		nlohmann::json root;

		//json文字列からjsonのデータ構造に展開
		ifs >> root;
		//ファイルを閉じる
		ifs.close();

		//グループを検索
		nlohmann::json::iterator itGroupUI = root.find(kItemName_);
		//未登録チェック
		assert(itGroupUI != root.end());

		texies_[static_cast<size_t>(scene.first)].clear();

		for (const auto& i : root[kItemName_][scene.second.c_str()]) {
			std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
			Vector3 pos,scale;
			Vector2 UVpos, UVscale;
			from_json(i["scale"], scale);
			from_json(i["translate"], pos);
			from_json(i["UVScale"], UVscale);
			from_json(i["UVTranslate"], UVpos);
			setTex_->color = i["color"];
			setTex_->transform.scale = scale;
			setTex_->transform.translate = pos;
			setTex_->uvTransform.scale = UVscale;
			setTex_->uvTransform.translate = UVpos;
			setTex_->textureID = DrawerManager::GetInstance()->LoadTexture(i["TextureFullPath"]);
			setTex_->textureFullPath = i["TextureFullPath"];
			setTex_->textureName = i["TextureName"];

			texies_[static_cast<size_t>(scene.first)].emplace_back(std::move(setTex_));

		}


	}



#ifdef _DEBUG
	/*std::string message = "File ALL loading completed";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);*/

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

void UIEditor::from_json(const json& j, Vector2& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
}

void UIEditor::from_json(const json& j, Vector3& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}
