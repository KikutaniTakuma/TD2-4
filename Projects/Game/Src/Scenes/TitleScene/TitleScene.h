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
#include"Scenes/TitleScene/TitleDirection.h"

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


	//始点と終点の位置関係で第二、第三点目の座標を設定
	void SetControlPoint();

	void Debug();

private:
	Texture2D* tex2D_;
	
	TitleDirection*  titleDirection_;

	Vector2 shakePower_;

	bool isCollision_;

	Audio* titleBGM_ = nullptr;

	Audio* beginGame_ = nullptr;

	const Vector2 minusPos_ = { -540.0f,-260.0f };

	Vector2 secondPointPos_ = { 50.0f,100.0f };

	Vector2 thirdPointPos_ = { 100.0f,50.0f };

	Vector2 controlPoint_[4] = {
		{100.0f + minusPos_.x,100.0f + minusPos_.y},
		{400.0f + minusPos_.x,400.0f + minusPos_.y},
		{700.0f + minusPos_.x,100.0f + minusPos_.y},
		{1000.0f + minusPos_.x,200.0f + minusPos_.y}
	};


};