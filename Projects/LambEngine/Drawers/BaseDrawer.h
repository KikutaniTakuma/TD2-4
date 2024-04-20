#pragma once
#include "Math/Mat4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <string>
#include "Engine/Graphics/GraphicsStructs.h"

class BaseDrawer {
public:
	BaseDrawer();
	BaseDrawer(const BaseDrawer&) = default;
	BaseDrawer(BaseDrawer&&) = default;
	virtual ~BaseDrawer() = default;

public:
	BaseDrawer& operator=(const BaseDrawer&) = default;
	BaseDrawer& operator=(BaseDrawer&&) = default;

public:
	virtual void Draw(const Mat4x4& worldMatrix, const Mat4x4& camera, uint32_t color, BlendType blend);

protected:
	class RenderSet* renderSet;
};

