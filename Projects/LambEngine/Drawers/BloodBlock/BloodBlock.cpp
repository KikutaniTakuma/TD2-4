#include "BloodBlock.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr/SafePtr.h"

const LoadFileNames BloodBlock::kFileNames =
	LoadFileNames{
		.resourceFileName{"./Resources/Cube.obj"},
		.shaderName{
			.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
			.psFileName = "./Resources/Shaders/BloodBlockShader/BloodBlock.PS.hlsl"
		}
};

void BloodBlock::Load() {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load<RenderContextType>(kFileNames);

	renderSet = renderContextManager->Get(kFileNames);

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

void BloodBlock::Draw(
	const Mat4x4& worldMatrix, 
	const Mat4x4& camera, 
	uint32_t color, 
	BlendType blend,
	const Vector3& bloodVector,
	bool isBlood
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<RenderContextType>(blend);

	renderContext->SetSahderStruct(
		ShaderData{
			.bloodVector = bloodVector,
			.isBlood = static_cast<uint32_t>(isBlood)
		}
	);

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}
