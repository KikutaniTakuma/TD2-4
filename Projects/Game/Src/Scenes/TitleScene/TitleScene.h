#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing/Easeing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Game/CollisionManager/Sphere/Sphere.h"

class TitleScene : public BaseScene {
public:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	~TitleScene() = default;

	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	//キャトムルロム曲線の描画
	void DrawCatmullRom(const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2, 
		const Vector2& controlPoint3, const Mat4x4& viewProjectionMatrix);

	//テクスチャの動き
	void MoveTexture();

	//始点と終点の位置関係で第二、第三点目の座標を設定
	void SetControlPoint();

	void Debug();

public:
	Texture2D* tex2D_;

	std::vector< std::unique_ptr<Tex2DState>> spheres_;

	//線上を走るテクスチャ
	std::unique_ptr<Tex2DState> runTexture_;

	bool isCollision_;

	Audio* titleBGM_ = nullptr;

	Audio* beginGame_ = nullptr;


	/*CatmullRom*/
	std::vector<std::unique_ptr<Line>> lines_;

	const Vector2 minusPos_ = { -540.0f,-260.0f };

	Vector2 secondPointPos_ = { 50.0f,100.0f };

	Vector2 thirdPointPos_ = { 100.0f,50.0f };

	Vector2 controlPoint_[4] = {
		{100.0f + minusPos_.x,100.0f + minusPos_.y},
		{400.0f + minusPos_.x,400.0f + minusPos_.y},
		{700.0f + minusPos_.x,100.0f + minusPos_.y},
		{1000.0f + minusPos_.x,200.0f + minusPos_.y}
	};
	//制御点
	std::vector<Vector2> ControlPoints_;

	std::vector<float> moveSpeeds_;

	//線の色
	uint32_t Linecolor_ = 0x000000ff;
	//分割数
	const uint32_t divisionNumber_ = 8;

	float point_ = 0.0f;
	//補完変数
	float t_ = 0.0f;


	//現在移動しているライン
	uint32_t linePass_ = 0;
	//初期ライン
	const uint32_t firstLinePass_ = 0;
	//最終ライン
	uint32_t LastLinePass_ = 2;
	//移動するかどうか
	bool isMove_ = false;
	//描画している数
	uint32_t drawCount_ = 0;
	//追加する数
	int addElementsNum_ = 1;
	//減らす数
	int subtractionElementsNum_ = 1;
	//下のenumに対応させるための変数
	int mode_ = 0;

	enum {
		First,
		Last,
	};

	//曲線一本の長さを調べるための変数群
	std::vector<float> catMullLength_;

	float length_;


};