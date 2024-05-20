#include "TitleDirection.h"

TitleDirection::~TitleDirection()
{
}

void TitleDirection::Initialize(){
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	catmullRom_ = std::make_unique<CatmullRomEditor>();
	catmullRom_->Initialize();

	ControlPoints_ = catmullRom_->GetControlPoints();
	moveSpeeds_ = catmullRom_->GetMoveSpeeds();

	/*fade_ = std::make_unique<Fade>();
	fade_->SetTransZ(-0.2f);
	fade_->SetInOutColor(0x00, 0xffffffff);
	fade_->SetFadeTime(0.5f);*/

	TextureInitialize();

	ti_Status_.easePos = { 500.0f,120.0f };
	e_Status_.easePos = { -500.0,100.0f };
	i_Status_.easePos = { 500.0f,120.0f };
	n_Status_.easePos = { 500.0f,120.0f };

	ti_Status_.easing = std::make_unique<Easeing>();
	e_Status_.easing = std::make_unique<Easeing>();
	i_Status_.easing = std::make_unique<Easeing>();
	n_Status_.easing = std::make_unique<Easeing>();

	isMoveTex_e_ = true;
}

void TitleDirection::TextureInitialize(){
	titleTex_ti_ = std::make_unique<Tex2DState>();
	titleTex_ti_->color = 0xffffffff;
	titleTex_ti_->transform.scale = texScale_;
	titleTex_ti_->transform.translate = { -200.0f, 120.0f };
	titleTex_ti_->uvTrnasform.scale = { 0.25f,1.0f };
	titleTex_ti_->uvTrnasform.translate = { 0.0f, 0.0f };
	titleTex_ti_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleChain.png");
	titleTex_ti_->textureFullPath = "./Resources/UI/titleChain.png";
	titleTex_ti_->textureName = "titleChain";

	titleTex_e_ = std::make_unique<Tex2DState>();
	titleTex_e_->color = 0xffffffff;
	titleTex_e_->transform.scale = texScale_;
	titleTex_e_->transform.translate = { -75.0f, 100.0f };
	titleTex_e_->uvTrnasform.scale = { 0.25f,1.0f };
	titleTex_e_->uvTrnasform.translate = { 0.25f, 0.0f };
	titleTex_e_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleChain.png");
	titleTex_e_->textureFullPath = "./Resources/UI/titleChain.png";
	titleTex_e_->textureName = "titleChain";

	titleTex_i_ = std::make_unique<Tex2DState>();
	titleTex_i_->color = 0xffffffff;
	titleTex_i_->transform.scale = texScale_;
	titleTex_i_->transform.translate = { 50.0f, 120.0f };
	titleTex_i_->uvTrnasform.scale = { 0.25f,1.0f };
	titleTex_i_->uvTrnasform.translate = { 0.5f, 0.0f };
	titleTex_i_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleChain.png");
	titleTex_i_->textureFullPath = "./Resources/UI/titleChain.png";
	titleTex_i_->textureName = "titleChain";

	titleTex_n_ = std::make_unique<Tex2DState>();
	titleTex_n_->color = 0xffffffff;
	titleTex_n_->transform.scale = texScale_;
	titleTex_n_->transform.translate = { 185.0f, 120.0f };
	titleTex_n_->uvTrnasform.scale = { 0.25f,1.0f };
	titleTex_n_->uvTrnasform.translate = { 0.75f, 0.0f };
	titleTex_n_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleChain.png");
	titleTex_n_->textureFullPath = "./Resources/UI/titleChain.png";
	titleTex_n_->textureName = "titleChain";

	titleTex_hut_ = std::make_unique<Tex2DState>();
	titleTex_hut_->color = 0xffffffff;
	titleTex_hut_->transform.scale = texScale_;
	titleTex_hut_->transform.translate = { 10.0f, 10.0f };
	titleTex_hut_->uvTrnasform.scale.kIdentity;
	titleTex_hut_->uvTrnasform.translate = { 0.0f, 0.0f };
	titleTex_hut_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleHat.png");
	titleTex_hut_->textureFullPath = "./Resources/UI/titleHat.png";
	titleTex_hut_->textureName = "titleHat";

	titleTex_text_ = std::make_unique<Tex2DState>();
	titleTex_text_->color = 0xffffff00;
	titleTex_text_->transform.scale = texScale_;
	titleTex_text_->transform.translate = { 10.0f, 10.0f };
	titleTex_text_->uvTrnasform.scale.kIdentity;
	titleTex_text_->uvTrnasform.translate = { 0.0f, 0.0f };
	titleTex_text_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/titleWitch.png");
	titleTex_text_->textureFullPath = "./Resources/UI/titleWitch.png";
	titleTex_text_->textureName = "titleWitch";

	titleTex_start_ = std::make_unique<Tex2DState>();
	titleTex_start_->color = 0xffffff00;
	titleTex_start_->transform.scale = { 256.0f,128.0f };
	titleTex_start_->transform.translate = { 0.0f, -200.0f };
	titleTex_start_->uvTrnasform.scale.kIdentity;
	titleTex_start_->uvTrnasform.translate = { 0.0f, 0.0f };
	titleTex_start_->textureID = DrawerManager::GetInstance()->LoadTexture("./Resources/UI/gameStartUi.png");
	titleTex_start_->textureFullPath = "./Resources/UI/gameStartUi.png";
	titleTex_start_->textureName = "gameStartUi";
}

void TitleDirection::Finalize(){

}

void TitleDirection::Update(){
	catmullRom_->Update();
	ControlPoints_ = catmullRom_->GetControlPoints();
	moveSpeeds_ = catmullRom_->GetMoveSpeeds();
	//SetControlPoint();

	MoveTextureChain();

	MoveTextureHut();


	ti_Status_.easing->Update();
	e_Status_.easing ->Update();
	i_Status_.easing ->Update();
	n_Status_.easing ->Update();

	if (e_Status_.easing->ActiveExit()) {
		isMoveTex_others_ = true;
	}

	if (n_Status_.easing->ActiveExit()) {
		linePass_ = 0;
		isMove_ = true;
	}

	titleTex_ti_->transform.translate.y = ti_Status_.easing->Get(ti_Status_.easePos.x, ti_Status_.easePos.y);
	titleTex_e_->transform.translate.y = e_Status_.easing->Get(e_Status_.easePos.x, e_Status_.easePos.y);
	titleTex_i_->transform.translate.y = i_Status_.easing->Get(i_Status_.easePos.x, i_Status_.easePos.y);
	titleTex_n_->transform.translate.y = n_Status_.easing->Get(n_Status_.easePos.x, n_Status_.easePos.y);


	titleTex_text_->transform.translate = titleTex_hut_->transform.translate;
	/*if (isFirstFade_){
		fade_->Update();
		FadeProcess();
	}*/
	titleTex_ti_->transform.CalcMatrix();
	titleTex_e_->transform.CalcMatrix();
	titleTex_i_->transform.CalcMatrix();
	titleTex_n_->transform.CalcMatrix();
	titleTex_text_->transform.CalcMatrix();
	titleTex_hut_->transform.CalcMatrix();
	titleTex_start_->transform.CalcMatrix();
	titleTex_ti_->uvTrnasform.CalcMatrix();
	titleTex_e_->uvTrnasform.CalcMatrix();
	titleTex_i_->uvTrnasform.CalcMatrix();
	titleTex_n_->uvTrnasform.CalcMatrix();
	titleTex_text_->uvTrnasform.CalcMatrix();
	titleTex_hut_->uvTrnasform.CalcMatrix();
	titleTex_start_->uvTrnasform.CalcMatrix();
}

void TitleDirection::Draw(const Mat4x4& cameraMat){
	tex2D_->Draw(titleTex_ti_->transform.matWorld_, titleTex_ti_->uvTrnasform.matWorld_, cameraMat
		, titleTex_ti_->textureID, titleTex_ti_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_e_->transform.matWorld_, titleTex_e_->uvTrnasform.matWorld_, cameraMat
		, titleTex_e_->textureID, titleTex_e_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_i_->transform.matWorld_, titleTex_i_->uvTrnasform.matWorld_, cameraMat
		, titleTex_i_->textureID, titleTex_i_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_n_->transform.matWorld_, titleTex_n_->uvTrnasform.matWorld_, cameraMat
		, titleTex_n_->textureID, titleTex_n_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_hut_->transform.matWorld_,titleTex_hut_->uvTrnasform.matWorld_, cameraMat
		, titleTex_hut_->textureID, titleTex_hut_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_text_->transform.matWorld_, titleTex_text_->uvTrnasform.matWorld_, cameraMat
		, titleTex_text_->textureID, titleTex_text_->color, BlendType::kNormal);

	tex2D_->Draw(titleTex_start_->transform.matWorld_, titleTex_start_->uvTrnasform.matWorld_, cameraMat
		, titleTex_start_->textureID, titleTex_start_->color, BlendType::kNormal);


	//fade_->Draw(cameraMat);

#ifdef _DEBUG

	catmullRom_->Draw(cameraMat);
#endif // _DEBUG

}

void TitleDirection::MoveTextureChain(){
	if (isMoveTex_e_){
		e_Status_.easing->Start(false,1.0f,Easeing::InOutBack);
		isMoveTex_e_ = false;
	}
	
	if (isMoveTex_others_) {
		ti_Status_.easing->Start(false, 0.3f, Easeing::InSine);
		i_Status_.easing->Start(false, 0.35f, Easeing::InSine);
		n_Status_.easing->Start(false, 0.35f, Easeing::InSine);
		isMoveTex_others_ = false;
	}



	/*if (e_Status_.easing->ActiveExit()) {
		isMoveTex_e_ = false;
	}
	if (i_Status_.easing->ActiveExit()) {
		isMoveTex_others_ = false;
	}*/
}

void TitleDirection::MoveTextureHut(){
	if (isMove_) {
		t_ += moveSpeeds_[linePass_];
	}
	if (t_ >= 1.0f) {
		t_ = 0.0f;
		linePass_++;
	}

	if (linePass_ == 0) {
		titleTex_hut_->transform.translate = Vector2::CatmullRom(ControlPoints_[0], ControlPoints_[0],
			ControlPoints_[1], ControlPoints_[2], t_);
	}
	else if (linePass_ >= 1 && linePass_ < catmullRom_->GetLastLinePass()) {
		titleTex_hut_->transform.translate = Vector2::CatmullRom(ControlPoints_[linePass_ - 1], ControlPoints_[linePass_],
			ControlPoints_[linePass_ + 1], ControlPoints_[linePass_ + 2], t_);
	}
	else if (linePass_ == catmullRom_->GetLastLinePass()) {
		titleTex_hut_->transform.translate = Vector2::CatmullRom(ControlPoints_[catmullRom_->GetLastLinePass() - 1], ControlPoints_[catmullRom_->GetLastLinePass()],
			ControlPoints_[catmullRom_->GetLastLinePass() + 1], ControlPoints_[catmullRom_->GetLastLinePass() + 1], t_);
	}
	else {
		isMove_ = false;
		/*if (not fade_->IsActive()){
			fade_->OutStart();

		}*/
	}
}

void TitleDirection::FadeProcess(){
	/*if (fade_->OutEnd()){
		titleTex_text_->color = 0xffffffff;
		titleTex_start_->color = 0xffffffff;
		fade_->InStart();
	}

	if (fade_->InEnd()){
		isFirstFade_ = false;
	}*/
}

void TitleDirection::SetControlPoint(){

}

void TitleDirection::Debug(){
#ifdef _DEBUG
	catmullRom_->Debug();

	ImGui::Begin("帽子の動き");

	if (ImGui::Button("テクスチャを動かす")) {
		isMove_ = true;
		linePass_ = 0;
	}

	ImGui::Text("LinePass = %d", linePass_);

	ImGui::End();

	ImGui::Begin("タイトルの動き");

	if (ImGui::Button("全体をリセットする")) {
		isMoveTex_e_ = true;
		e_Status_.easing->Restart();
		ti_Status_.easing->Start(false, 0.3f, Easeing::InSine);
		i_Status_.easing->Start(false, 0.35f, Easeing::InSine);
		n_Status_.easing->Start(false, 0.35f, Easeing::InSine);
		ti_Status_.easing->Stop();
		i_Status_.easing->Stop();
		n_Status_.easing->Stop();
		isMove_ = false;
		isMoveTex_others_ = false;
		linePass_ = 0;
	}
	if (ImGui::Button("テクスチャ[ェ]以外を動かす")) {
		isMoveTex_others_ = true;
		ti_Status_.easing->Restart();
		i_Status_.easing->Restart();
		n_Status_.easing->Restart();
	}
	

	ImGui::End();

	ImGui::Begin("各テクスチャの大きさ座標");
	ImGui::DragFloat2("チの大きさ", titleTex_ti_->transform.scale.data(), 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("チの座標", titleTex_ti_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("チのUV大きさ", titleTex_ti_->uvTrnasform.scale.data(), 0.001f, 0.0f, 1280.0f);
	ImGui::DragFloat2("チのUV座標", titleTex_ti_->uvTrnasform.translate.data(), 0.001f, 0.0f, 1280.0f);

	ImGui::DragFloat2("ェの大きさ", titleTex_e_->transform.scale.data(), 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("ェの座標", titleTex_e_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("ェのUV大きさ", titleTex_e_->uvTrnasform.scale.data(), 0.001f, 0.0f, 1280.0f);
	ImGui::DragFloat2("ェのUV座標", titleTex_e_->uvTrnasform.translate.data(), 0.001f, 0.0f, 1280.0f);

	ImGui::DragFloat2("イの大きさ", titleTex_i_->transform.scale.data(), 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("イの座標", titleTex_i_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("イのUV大きさ", titleTex_i_->uvTrnasform.scale.data(), 0.001f, 0.0f, 1280.0f);
	ImGui::DragFloat2("イのUV座標", titleTex_i_->uvTrnasform.translate.data(), 0.001f, 0.0f, 1280.0f);

	ImGui::DragFloat2("ンの大きさ", titleTex_n_->transform.scale.data(), 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("ンの座標", titleTex_n_->transform.translate.data(), 1.0f);
	ImGui::DragFloat2("ンのUV大きさ", titleTex_n_->uvTrnasform.scale.data(), 0.001f, 0.0f, 1280.0f);
	ImGui::DragFloat2("ンのUV座標", titleTex_n_->uvTrnasform.translate.data(), 0.001f, 0.0f, 1280.0f);



	ImGui::End();
#endif // 
}
