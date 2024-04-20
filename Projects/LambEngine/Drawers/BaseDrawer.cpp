#include "BaseDrawer.h"
#include "Utils/SafePtr/SafePtr.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


BaseDrawer::BaseDrawer() :
	renderSet(nullptr)
{}

void BaseDrawer::Draw(const Mat4x4& worldMatrix, const Mat4x4& camera, uint32_t color, BlendType blend)
{
	Lamb::SafePtr render = renderSet->GetRenderContext(blend);

	render->SetWVPMatrix({
		.worldMat = worldMatrix,
		.cameraMat = camera
		}
	);

	render->SetColor(color);

	render->AddDrawCount();
}
