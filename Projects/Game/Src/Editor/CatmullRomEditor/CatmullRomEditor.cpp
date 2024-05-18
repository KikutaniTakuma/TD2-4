#include "CatmullRomEditor.h"
#include"Engine/Core/WindowFactory/WindowFactory.h"
#include <fstream>


CatmullRomEditor::CatmullRomEditor(){

}

void CatmullRomEditor::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	if (not LoadFiles()) {
		for (int i = 0; i < 4; i++) {
			std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
			setTex_->color = 0x000000ff;
			setTex_->transform.scale = { 10.0f, 10.0f };
			setTex_->transform.translate = { 10.0f, 10.0f };
			setTex_->uvTrnasform.scale = { 0.0f, 0.0f };
			setTex_->uvTrnasform.translate = { 0.0f, 0.0f };
			setTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
			setTex_->textureFullPath = "./Resources/Ball.png";
			setTex_->textureName = "Ball";
			spheres_.emplace_back(std::move(setTex_));

			ControlPoints_.push_back(controlPoint_[i]);
		}
	}
	else {
		if (ControlPoints_.size() < 4) {
			for (int i = 0; i < 4; i++) {
				std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
				setTex_->color = 0x000000ff;
				setTex_->transform.scale = { 10.0f, 10.0f };
				setTex_->transform.translate = { 10.0f, 10.0f };
				setTex_->uvTrnasform.scale = { 0.0f, 0.0f };
				setTex_->uvTrnasform.translate = { 0.0f, 0.0f };
				setTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
				setTex_->textureFullPath = "./Resources/Ball.png";
				setTex_->textureName = "Ball";
				spheres_.emplace_back(std::move(setTex_));

				ControlPoints_.push_back(controlPoint_[i]);
			}
		}
		
	}

	for (size_t i = 0; i < (ControlPoints_.size() - 1) * divisionNumber_; i++) {
		lines_.push_back(std::make_unique<Line>());
	}

	LastLinePass_ = static_cast<int>(ControlPoints_.size()) - 2;

	mode_ = First;

}

void CatmullRomEditor::Update(){
	for (size_t i = 0; i < spheres_.size(); i++) {
		spheres_[i]->transform.translate = ControlPoints_[i];
		spheres_[i]->transform.CalcMatrix();
	}
}

void CatmullRomEditor::Draw(const Mat4x4& cameraMat){
	drawCount_ = 0;
	for (size_t j = 0; j < spheres_.size(); j++) {
		tex2D_->Draw(spheres_[j]->transform.matWorld_, Mat4x4::kIdentity, cameraMat
			, spheres_[j]->textureID, spheres_[j]->color, BlendType::kNormal);
	}

	DrawCatmullRom(ControlPoints_[0], ControlPoints_[0], ControlPoints_[1], ControlPoints_[2], cameraMat);
	for (uint32_t i = 1; i < LastLinePass_; i++) {
		DrawCatmullRom(ControlPoints_[i - 1], ControlPoints_[i], ControlPoints_[i + 1], ControlPoints_[i + 2], cameraMat);
	}

	DrawCatmullRom(ControlPoints_[LastLinePass_ - 1], ControlPoints_[LastLinePass_], ControlPoints_[LastLinePass_ + 1], ControlPoints_[LastLinePass_ + 1], cameraMat);

}

void CatmullRomEditor::Debug(){
#ifdef _DEBUG
	ImGui::Begin("CatmullRomエディター", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()){
		if (ImGui::BeginMenu("制御点の座標")) {
			for (size_t i = 0; i < ControlPoints_.size(); ++i) {
				ImGui::DragFloat2(("Points" + std::to_string(i)).c_str(), ControlPoints_[i].data(), 1.0f);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("線の増減")) {			
			ImGui::InputInt("何個追加するか", &addElementsNum_);

			if (addElementsNum_ < 1) {
				addElementsNum_ = 1;
			}

			if (ImGui::Button("線の追加")) {
				for (int y = 0; y < addElementsNum_; y++) {
					Vector2 newPoint = { 0.0f,0.0f };

					LastLinePass_++;

					ControlPoints_.push_back(newPoint);
					if ((ControlPoints_.size() - 1) * divisionNumber_ > lines_.size()) {
						for (uint32_t i = 0; i < divisionNumber_; i++) {
							lines_.push_back(std::make_unique<Line>());
						}
					}

					std::unique_ptr<Tex2DState> setTex_ = std::make_unique<Tex2DState>();
					setTex_->color = 0x000000ff;
					setTex_->transform.scale = { 10.0f, 10.0f };
					setTex_->transform.translate = newPoint;
					setTex_->uvTrnasform.scale = { 0.0f, 0.0f };
					setTex_->uvTrnasform.translate = { 0.0f, 0.0f };
					setTex_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
					setTex_->textureFullPath = "./Resources/Ball.png";
					setTex_->textureName = "Ball";
					spheres_.emplace_back(std::move(setTex_));

				}
			}

			ImGui::InputInt("何個削除するか", &subtractionElementsNum_);

			if (subtractionElementsNum_ < 1) {
				subtractionElementsNum_ = 1;
			}

			if (ImGui::Button("線の削除")) {
				if (mode_ == First) {
					for (int i = 0; i < subtractionElementsNum_; i++) {
						if (ControlPoints_.size() <= 4) {
							break;
						}
						if (!ControlPoints_.empty()) {
							LastLinePass_--;

							ControlPoints_.erase(ControlPoints_.begin());

							if ((ControlPoints_.size() - 1) * 8 < lines_.size()) {
								for (int j = 0; j < 8; j++) {
									lines_.erase(lines_.begin());
								}
							}

							spheres_.erase(spheres_.begin());
						}
					}
				}
				
			}

			if (ImGui::Button("最後の線を削除")) {
				if (!ControlPoints_.empty()) {
					LastLinePass_--;

					ControlPoints_.pop_back();
					if ((ControlPoints_.size() - 1) * 8 < lines_.size()) {
						for (int j = 0; j < 8; j++) {
							lines_.pop_back();
						}
					}
					spheres_.pop_back();
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ファイル関連")) {
			if (ImGui::Button("現在のポジションで保存する")) {
				if (OperationConfirmation()) {
					SaveFile();
				}
			}
			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}
	ImGui::End();

#endif // _DEBUG
}

void CatmullRomEditor::DrawCatmullRom(const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2, 
	const Vector2& controlPoint3, const Mat4x4& viewProjectionMatrix){
	//分割数
	const uint32_t divisionNumber = 8;
	//曲線の変数
	Vector2 CatmullRom[divisionNumber + 1] = {};
	float t = 0.0f;
	float lengthNum = 0.0f;



	for (uint32_t i = 0; i < divisionNumber + 1; i++) {
		t = i / static_cast<float>(divisionNumber);

		Vector2 p = Vector2::CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t);

		CatmullRom[i] = p;

	}

	for (uint32_t i = 0; i < divisionNumber; i++) {
		Vector2 first_ = CatmullRom[i];
		Vector2 second_ = CatmullRom[i + 1];

		length_ = (second_ - first_).Length();
		lengthNum += length_;
		if (i + drawCount_ * divisionNumber < lines_.size()) {
			lines_[i + drawCount_ * divisionNumber]->Draw(first_, second_, viewProjectionMatrix, Linecolor_);
		}

	}
	if (catMullLength_.size() < ControlPoints_.size() - 1) {
		catMullLength_.push_back(lengthNum);
	}
	else {
		catMullLength_[drawCount_] = lengthNum;
	}

	drawCount_++;
}


void CatmullRomEditor::SaveFile(){
	json root;
	root = json::object();

	/*
	* 制御点の座標
	* 区間毎の速度
	*/

	for (size_t i = 0; i < ControlPoints_.size(); i++) {
		Vector2& point = ControlPoints_[i];

		root[kItemName_][i]["translate"] = json::array({
			   point.x,
			   point.y
			});

	}

	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::filesystem::create_directory(kDirectoryName_);
	}
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + kItemName_ + ".json";
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込みように開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		throw Lamb::Error::Code<CatmullRomEditor>(message, ErrorPlace);
	}
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

	std::string message = "File save completed.";
	MessageBoxA(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), "Object", 0);

}

void CatmullRomEditor::ChackFiles(){

}

bool CatmullRomEditor::LoadFiles(){
	if (!std::filesystem::exists(kDirectoryName_)) {
		std::wstring message = L"ファイルが見つかりませんでした。";
		MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"Lineないよぉ！", 0);
		return false;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	std::string readFilePath = kDirectoryPath_ + kItemName_ + ".json";

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		if (filePath == readFilePath) {
			//ファイル読み込み
			LoadFile(readFilePath);
			return true;
		}
	}
	std::wstring message = L"ファイルが見つかりませんでした。";
	MessageBoxW(WindowFactory::GetInstance()->GetHwnd(), message.c_str(), L"Lineないよぉ！", 0);
	return false;
}

bool CatmullRomEditor::LoadFile(const std::string& groupName){
	groupName;
	return false;
}

bool CatmullRomEditor::LoadChackItem(const std::string& directoryPath, const std::string& itemName){

	directoryPath; itemName;
	return false;
}

bool CatmullRomEditor::OperationConfirmation(){

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

void CatmullRomEditor::from_json(const json& j, Vector2& v){
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
}
