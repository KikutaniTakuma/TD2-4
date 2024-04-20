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
#include "Game/ItemManager/ItemManager.h"
#include "Game/CollisionManager/Sphere/Sphere.h"

class SelectScene : public BaseScene {
public:
	SelectScene();
	SelectScene(const SelectScene&) = delete;
	SelectScene(SelectScene&&) = delete;
	~SelectScene() = default;

	SelectScene& operator=(const SelectScene&) = delete;
	SelectScene& operator=(SelectScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void Debug();

	void SelectMove();

public:
	static int GetSelectNumber() { return selectNum_; }

	static void SetClearFlag(int x) { isStageClear_[x] = true; }

	static void ClearFlagReset() {
		for (int i = 0; i < maxStage_ - 1; i++) {
			isStageClear_[i] = false;
		}
	}
private:
	//ステージセレクト用の変数
	static int selectNum_;
	int oldSelectNum_ = 0;
	static const int maxStage_ = 10;

	static bool isStageClear_[maxStage_];

	const float stageInterbal = 100.0f;

	std::array<std::unique_ptr<Texture2D>,maxStage_> texies_;

	std::unique_ptr<Texture2D> selectTex_;
};

