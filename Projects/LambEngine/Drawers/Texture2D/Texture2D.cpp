#include "Texture2D.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr/SafePtr.h"

#ifdef _DEBUG
#include "Utils/UtilsLib/UtilsLib.h"

#include "imgui.h"
#endif // _DEBUG


const LoadFileNames Texture2D::kFileNames_ = 
		LoadFileNames{
			.resourceFileName{"./Resources/Texture2D/Texture2D.obj"},
			.shaderName{
				.vsFileName = "./Resources/Shaders/Texture2DShader/Texture2D.VS.hlsl",
				.psFileName = "./Resources/Shaders/Texture2DShader/Texture2D.PS.hlsl"
			}
		};

Texture2D::Texture2D():
	BaseDrawer()
{}

void Texture2D::Load()
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load<Texture2DRenderContext>(kFileNames_);

	renderSet = renderContextManager->Get(kFileNames_);
}

void Texture2D::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& uvTransform,
	const Mat4x4& camera,
	uint32_t textureID,
	uint32_t color,
	BlendType blend
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<Texture2DRenderContext>(blend);

	renderContext->SetSahderStruct(
		ShaderData{
			.uvTransform = uvTransform,
			.pad = Vector3::kZero,
			.textureID = textureID
		}
	);

	BaseDrawer::Draw(worldMatrix, camera, color,  blend);
}
