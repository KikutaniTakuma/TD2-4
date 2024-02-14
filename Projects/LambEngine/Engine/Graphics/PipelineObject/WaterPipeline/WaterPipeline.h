#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector3.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"

class WaterPipeline final : public PeraPipeline {
private:
	struct Normals {
		Vector3 normal;
		float pad;
		Vector3 tangent;
	};
public:
	WaterPipeline() = default;
	WaterPipeline(const WaterPipeline&) = delete;
	WaterPipeline(WaterPipeline&&) = delete;
	~WaterPipeline();

	WaterPipeline& operator=(const WaterPipeline&) = delete;
	WaterPipeline& operator=(WaterPipeline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/PostWater.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostWater.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

	void SetCameraPos(const Vector3& cameraPos) {
		light_->eyePos = cameraPos;
	}

private:
	ConstBuffer<Normals> normalVector_;
	ConstBuffer<Vector2> randomVec_;
	ConstBuffer<Mesh::Light> light_;
	ConstBuffer<float> densityScale_;

	class Texture* caustics_;

	Vector2 speed_;
};
