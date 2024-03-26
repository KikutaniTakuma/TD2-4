#include "Slime.h"
#include "Engine/Graphics/PipelineObject/SlimePipeline/SlimePipeline.h"

void Slime::Init() {
	body_ = std::make_unique<Texture2D>("./Resources/Ball.png");
	body_->isSameTexSize = true;
	body_->color = 0xD8D8D8A4;
	core_ = std::make_unique<Texture2D>("./Resources/Ball.png");
	core_->isSameTexSize = true;
	core_->texScalar *= 0.5f;
	core_->color = Vector4ToUint(Vector4{0.8f, 0.0f,0.0f,1.0f});

	slimePipeline_ = new SlimePipeline();
	slimePipeline_->Init();

	pera_ = std::make_unique<PeraRender>();
	pera_->Initialize(slimePipeline_);

	peraCamera_ = std::make_unique<Camera>();
	peraCamera_->pos.z = -0.01f;
	peraCamera_->Update();
}

void Slime::Update() {
	core_->Debug("slime core");
	core_->Update();

	body_->pos = core_->pos;
	body_->scale = core_->scale * 2.0f;

	body_->Debug("slime body");
	body_->Update();

	pera_->Update();
}

void Slime::Draw(const Camera& camera) {
	core_->Draw(camera.GetViewOthographics(), Pipeline::Normal, false);

	pera_->PreDraw();

	body_->Draw(camera.GetViewOthographics(), Pipeline::Normal, false);

	pera_->Draw(peraCamera_->GetViewOthographics(), Pipeline::Normal);
}
