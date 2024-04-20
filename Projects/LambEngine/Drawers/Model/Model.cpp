#include "Model.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr/SafePtr.h"

Model::Model(const std::string& fileName):
	Model()
{
	Load(fileName);
}

void Model::Load(const std::string& fileName) {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load(
		LoadFileNames{
			.resourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);

	renderSet = renderContextManager->Get(
		LoadFileNames{
			.resourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);

	for (auto& i : *renderSet) {
		i->SetLight(
			Light{
				.ligDirection{-Vector3::kYIdentity},
				.pad0{},
				.ligColor{ Vector3::kIdentity },
			}
		);
	}
}

void Model::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera, 
	uint32_t color, 
	BlendType blend, 
	bool isLighting
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(blend);
	renderContext->SetSahderStruct(static_cast<uint32_t>(isLighting));

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}