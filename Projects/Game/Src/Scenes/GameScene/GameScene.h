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

#include "../../../../SoLib/IO/File.h"
#include "../../../../SoLib/IO/CSV.h"
#include "../../../../SoLib/Containers/Array2D.h"

#include "GameObject/GameObject.h"
#include "Game/GameManager/GameManager.h"

#include "Game/CollisionManager/AABB/AABB.h"

#include "Editor/BlockEditor/BlockEditor.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene &) = delete;
	GameScene(GameScene &&) = delete;
	~GameScene() = default;

	GameScene &operator=(const GameScene &) = delete;
	GameScene &operator=(GameScene &&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	class Water *water_ = nullptr;

	GameManager *gameManager_ = nullptr;

	AABB aabb_;

	std::unique_ptr<Model> boxModel_;

	std::unique_ptr<SkyDome> skydome_ = nullptr;
	class Cloud *cloud_ = nullptr;

	std::unique_ptr<BlockEditor> blockEditor_;
};