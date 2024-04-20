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
	int GetSelectNumber() { return selectNum_; }

	void SetClearFlag(int x) { isStageClear_[x] = true; }

	void ClearFlagReset() {
		for (int i = 0; i < maxStage_ - 1; i++) {
			isStageClear_[i] = false;
		}
	}
private:
	//ステージセレクト用の変数
	int32_t selectNum_;
	int32_t oldSelectNum_ = 0;
	static constexpr int32_t maxStage_ = 10;

	std::array <bool,maxStage_> isStageClear_;

	const float stageInterbal = 100.0f;

	Texture2D* tex2D_;
	std::array<std::unique_ptr<Tex2DState>,maxStage_> texies_;

	std::unique_ptr<Tex2DState> selectTex_;
};

