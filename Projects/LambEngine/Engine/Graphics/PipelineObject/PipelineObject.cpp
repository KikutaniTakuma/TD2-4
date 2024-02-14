#include "PipelineObject.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include <cassert>

void PipelineObject::LoadShader(
	const std::string& vsFileName,
	const std::string& psFileName,
	const std::string& gsFileName,
	const std::string& hsFileName,
	const std::string& dsFileName
) {
	static ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader(vsFileName);
	shader_.pixel = shaderManager->LoadPixelShader(psFileName);
	shader_.geometory = shaderManager->LoadGeometoryShader(gsFileName);
	shader_.hull = shaderManager->LoadHullShader(hsFileName);
	shader_.domain = shaderManager->LoadDomainShader(dsFileName);
}