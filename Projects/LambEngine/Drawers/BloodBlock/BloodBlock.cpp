#include "BloodBlock.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr/SafePtr.h"

const LoadFileNames BloodBlock::kFileNames_ =
	LoadFileNames{
		.resourceFileName{"./Resources/Cube.obj"},
		.shaderName{
			.vsFileName = "./Resources/Shaders/BloodBlockShader/BloodBlock.VS.hlsl",
			.psFileName = "./Resources/Shaders/BloodBlockShader/BloodBlock.PS.hlsl"
		}
};

void BloodBlock::Load() {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load<RenderContextType>(kFileNames_);

	renderSet = renderContextManager->Get(kFileNames_);
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
			.isBlood = isBlood
		}
	);

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}
