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

	void DrawCatmullRom(const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2, 
		const Vector2& controlPoint3, const Mat4x4& viewProjectionMatrix);


	void Debug();

public:
	Texture2D* tex2D_;

	std::vector< std::unique_ptr<Tex2DState>> spheres_;


	bool isCollision_;

	Audio* titleBGM_ = nullptr;

	Audio* beginGame_ = nullptr;


	/*CatmullRom*/
	std::vector<std::unique_ptr<Line>> lines_;

	Vector2 controlPoint_[4] = {
		{100.0f,100.0f},
		{400.0f,400.0f},
		{700.0f,100.0f},
		{1000.0f,200.0f}
	};

	std::vector<Vector2> ControlPoints_;

	//分割数
	uint32_t Linecolor_ = 0x000000ff;

	const uint32_t divisionNumber_ = 8;
	float point_ = 0.0f;
	float t_ = 0.0f;

	uint32_t linePass_ = 0;

	const uint32_t firstLinePass_ = 0;
	uint32_t LastLinePass_ = 2;

	bool isMove_ = false;

	uint32_t drawCount_ = 0;

	int addElementsNum_ = 1;

	int subtractionElementsNum_ = 1;

	int mode_ = 0;

	enum {
		First,
		Last,
	};

	//曲線一本の長さを調べるための変数群
	std::vector<float> catMullLength_;

	float length_;


};