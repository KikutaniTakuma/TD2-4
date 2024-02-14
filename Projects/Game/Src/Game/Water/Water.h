#pragma once
#include <memory>
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "./Camera/Camera.h"
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"

class Water final {
private:
	Water() = default;

public:
	~Water() = default;

public:
	static void Initialize();

	static void Finalize();

	static Water* const GetInstance();

private:
	static Water* instance_;


public:
	void Init();

	void Update(const Vector3& cameraPos);

	void Draw(const Mat4x4& cameraMat);

	void Debug(const std::string& guiName);

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

private:
	std::unique_ptr<PeraRender> pera_;
	std::unique_ptr<PeraRender> luminate_;
	std::unique_ptr<PeraRender> bloom_;
	std::unique_ptr<Texture2D> waterSurface_;

	class WaterPipeline* waterPipelineObject_ = nullptr;

	std::unique_ptr<Camera> staticCamera_;
};