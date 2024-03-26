#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector3.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"

class SlimePipeline final : public PeraPipeline {
public:
	SlimePipeline() = default;
	SlimePipeline(const SlimePipeline&) = delete;
	SlimePipeline(SlimePipeline&&) = delete;
	~SlimePipeline();

	SlimePipeline& operator=(const SlimePipeline&) = delete;
	SlimePipeline& operator=(SlimePipeline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/Slime.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

private:
	ConstBuffer<Vector2> randomVec_;
	Vector2 firstRandomVec_;

	Vector2 speed_;
};
