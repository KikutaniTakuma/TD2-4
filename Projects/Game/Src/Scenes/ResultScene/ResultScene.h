#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Particle/Particle.h"
#include "Utils/Easeing/Easeing.h"
#include "Game/FlaskParticle/FlaskParticle.h"
#include "Math/Transform.h"


class ResultScene : public BaseScene{
public:
	ResultScene();
	ResultScene(const ResultScene&) = delete;
	ResultScene(ResultScene&&) = delete;
	~ResultScene() = default;

	ResultScene& operator=(const ResultScene&) = delete;
	ResultScene& operator=(ResultScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void Debug();
private:
	Lamb::SafePtr<Texture2D> tex2D_;
	Transform backGround_;
	uint32_t backGroundTextureID_;
	//std::unique_ptr<Particle> backGroundParticle_;
	std::unique_ptr<Easeing> backGroundEase_;
	Vector3 backGroundStartPos_;
	Vector3 backGroundEndPos_;
	Lamb::Flg isActiveParticle_;


	std::array<std::unique_ptr<FlaskParticle>, 4> flaskParticles_;
	std::array<uint32_t, 4> flaskTextureID_;
};

