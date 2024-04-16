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

#include "Game/CollisionManager/CollisionManager.h"

#include"Game/Player/Player.h"

#include "Editor/BlockEditor/BlockEditor.h"
#include "Editor/EnemyEditor/EnemyEditor.h"

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
	//class Water *water_ = nullptr;

	GameManager *gameManager_ = nullptr;

	std::unique_ptr<Player> player_;

	CollisionManager* collisionManager_ = nullptr;

	/*std::unique_ptr<SkyDome> skydome_ = nullptr;
	class Cloud *cloud_ = nullptr;*/

	bool editorMode_ = false;

	bool enemyMode_ = false;

	std::unique_ptr<BlockEditor> blockEditor_;

	std::unique_ptr<EnemyEditor> enemyEditor_;

	EnemyManager* enemyManager_ = nullptr;
};