#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector3.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"

class CloudPipeline final : public PeraPipeline {
private:
	struct CloudData {
		float density;

		// 指数
		float exponent;

		// 色のスカラー倍
		float colorTime;
	};

public:
	CloudPipeline() = default;
	CloudPipeline(const CloudPipeline&) = delete;
	CloudPipeline(CloudPipeline&&) = delete;
	~CloudPipeline();

	CloudPipeline& operator=(const CloudPipeline&) = delete;
	CloudPipeline& operator=(CloudPipeline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/PostCloud.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostCloud.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

private:
	ConstBuffer<Vector2> randomVec_;
	ConstBuffer<CloudData> cloudData_;
	ConstBuffer<Vector4> skyColor_;

	Vector2 speed_;
};
