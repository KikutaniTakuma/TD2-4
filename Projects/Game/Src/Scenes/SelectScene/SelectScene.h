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
private:
	std::unique_ptr<Texture2D> tex_;
};

