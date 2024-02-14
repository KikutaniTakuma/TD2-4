#pragma once
#include <string>
#include <unordered_map>
#include "../Shader/Shader.h"

#include "../PipelineManager/Pipeline/Pipeline.h"

class PipelineObject {
public:
	PipelineObject() = default;
	PipelineObject(const PipelineObject&) = delete;
	PipelineObject(PipelineObject&&) = delete;
	virtual ~PipelineObject() = default;

	PipelineObject& operator=(const PipelineObject&) = delete;
	PipelineObject& operator=(PipelineObject&&) = delete;

protected:
	void LoadShader(
		const std::string& vsFileName,
		const std::string& psFileName,
		const std::string& gsFileName,
		const std::string& hsFileName,
		const std::string& dsFileName
	);

public:
	virtual void Use(Pipeline::Blend blendType, bool isDepth) = 0;

	virtual void Init(
		const std::string& vsFileName, 
		const std::string& psFileName,
		const std::string& gsFileName,
		const std::string& hsFileName,
		const std::string& dsFileName
	) = 0;

protected:
	Shader shader_;

	std::unordered_map<Pipeline::Blend, Pipeline*> pipelines_;
	std::unordered_map<Pipeline::Blend, Pipeline*> pipelinesNoDepth_;
};