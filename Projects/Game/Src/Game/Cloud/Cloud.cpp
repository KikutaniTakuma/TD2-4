#include "Cloud.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "Engine/Graphics/PipelineObject/CloudPipeline/CloudPipeline.h"
#include "Utils/EngineInfo/EngineInfo.h"


Cloud* Cloud::instance_ = nullptr;

void Cloud::Initialize()
{
	if (!instance_) {
		instance_ = new Cloud{};
		instance_->Init();
	}
}

void Cloud::Finalize()
{
	if (instance_) {
		Lamb::SafeDelete(instance_);
	}
}

Cloud* const Cloud::GetInstance()
{
	return instance_;
}

void Cloud::Init()
{
	pipeline_ = new CloudPipeline{};
	pipeline_->Init();

	Vector2 texSize_ = Vector2::kIdentity * 1280.0f;

	pera_.reset(new PeraRender{static_cast<uint32_t>(texSize_.x),static_cast<uint32_t>(texSize_.y) });
	pera_->Initialize(pipeline_);
	pera_->scale = Lamb::ClientSize();

	pera2_.reset(new PeraRender{  });
	pera2_->Initialize("./Resources/Shaders/PostShader/PostNone.PS.hlsl");

	pera2_->scale = Lamb::ClientSize();


	staticCamera_.reset(new Camera{});
	staticCamera_->pos.z = -0.1f;
	staticCamera_->Update();

	tex_.reset(new Texture2D{});
	tex_->scale = texSize_;
}

void Cloud::Update()
{
	pera_->Update();

	tex_->Update();

	pera2_->Update();
}

void Cloud::Draw()
{
	pera_->PreDraw();
	tex_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, false);
	pera_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, pera2_.get());
	pera2_->SetMainRenderTarget();

}
