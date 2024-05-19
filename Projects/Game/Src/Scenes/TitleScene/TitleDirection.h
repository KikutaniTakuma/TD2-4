#pragma once
#include "Editor/CatmullRomEditor/CatmullRomEditor.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"

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

	void Initialize() ;

	void TextureInitialize();

	void Finalize() ;

	void Update() ;

	void Draw(const Mat4x4& cameraMat) ;

	//テクスチャの動き
	void MoveTextureChain();

	void MoveTextureHut();

	//始点と終点の位置関係で第二、第三点目の座標を設定
	void SetControlPoint();

	void Debug();

private:
	//Y座標だけのイージング
	struct TitleEaseStatus{
		std::unique_ptr<Easeing> easing;
		//始点x、終点y
		Vector2 easePos;
	};

	Texture2D* tex2D_;

	std::unique_ptr<CatmullRomEditor> catmullRom_;

	//タイトルテクスチャ
	//チの箱
	std::unique_ptr<Tex2DState> titleTex_ti_;
	TitleEaseStatus ti_Status_;

	bool isMoveTex_e_ = false;

	bool isMoveTex_others_ = false;
	
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

	Vector2 texScale_ = { 128.0f,128.0f };


	const Vector2 minusPos_ = { -540.0f,-260.0f };

	Vector2 secondPointPos_ = { 50.0f,100.0f };

	Vector2 thirdPointPos_ = { 100.0f,50.0f };

	Vector2 controlPoint_[4] = {
		{100.0f + minusPos_.x,100.0f + minusPos_.y},
		{400.0f + minusPos_.x,400.0f + minusPos_.y},
		{700.0f + minusPos_.x,100.0f + minusPos_.y},
		{1000.0f + minusPos_.x,200.0f + minusPos_.y}
	};

private:

	//制御点
	std::vector<Vector2> ControlPoints_;

	std::vector<float> moveSpeeds_;



	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;


	//現在移動しているライン
	uint32_t linePass_ = 0;
	//移動するかどうか
	bool isMove_ = false;
};

