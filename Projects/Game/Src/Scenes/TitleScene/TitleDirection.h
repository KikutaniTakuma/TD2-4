#pragma once
#include "Editor/CatmullRomEditor/CatmullRomEditor.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include"Scenes/Manager/SceneManager.h"

#include"Scenes/Manager/Fade/SimpleFade.h"

class TitleDirection{
public:
	TitleDirection() = default;
	TitleDirection(const TitleDirection&) = delete;
	TitleDirection(TitleDirection&&) = delete;
	~TitleDirection();

	TitleDirection& operator=(const TitleDirection&) = delete;
	TitleDirection& operator=(TitleDirection&&) = delete;

public:
	static TitleDirection* const GetInstance() {
		static TitleDirection instance;
		return &instance;
	}

	void Initialize(SceneManager* sceneManager) ;

	void TextureInitialize();

	void Finalize() ;

	void Update(Input* input);

	void Draw(const Mat4x4& cameraMat) ;

	//テクスチャの動き
	void MoveTextureChain();

	void MoveTextureHut();
	//フェードの処理
	void FadeProcess();

	//始点と終点の位置関係で第二、第三点目の座標を設定
	void SetControlPoint();

	const bool GetIsActiveExit() {
		return n_Status_.easing->ActiveExit();
	}

	const bool GetIsFirstFade() {
		return isFirstFade_;
	}

	void Debug();

private:
	//Y座標だけのイージング
	struct TitleEaseStatus{
		std::unique_ptr<Easeing> easing;
		//始点x、終点y
		Vector2 easePos;
	};

	SceneManager* sceneManager_ = nullptr;

	Texture2D* tex2D_;

	Audio* putBlock_ = nullptr;

	std::unique_ptr<SimpleFade> fade_;

	std::unique_ptr<CatmullRomEditor> catmullRom_;

	bool isMoveTex_e_ = false;

	bool isMoveTex_others_ = false;

	bool isShake_ = false;
	//タイトルテクスチャ
	//チの箱
	std::unique_ptr<Tex2DState> titleTex_ti_;
	TitleEaseStatus ti_Status_;	
	//ェの箱
	std::unique_ptr<Tex2DState> titleTex_e_;
	TitleEaseStatus e_Status_;
	//イの箱
	std::unique_ptr<Tex2DState> titleTex_i_;
	TitleEaseStatus i_Status_;
	//ンの箱
	std::unique_ptr<Tex2DState> titleTex_n_;
	TitleEaseStatus n_Status_;
	//帽子
	std::unique_ptr<Tex2DState> titleTex_hut_;
	//魔女娘テキスト
	std::unique_ptr<Tex2DState> titleTex_text_;

	std::unique_ptr<Tex2DState> titleTex_start_;

	std::unique_ptr<Tex2DState> titleTex_startText_;


	Vector2 texScale_ = { 128.0f,128.0f };

	Vector2 texHutScale_ = { 192.0f,192.0f };

private:

	//制御点
	std::vector<Vector2> ControlPoints_;

	std::vector<float> moveSpeeds_;


	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;

	uint32_t time_;


	//現在移動しているライン
	uint32_t linePass_ = 0;
	//移動するかどうか
	bool isMove_ = false;
	//初回のフェードかどうか
	bool isFirstFade_ = true;

	bool isDirectionSkep_ = false;
};

