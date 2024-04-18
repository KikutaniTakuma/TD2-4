#pragma once
#include "../PipelineObject.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Math/Mat4x4.h"
#include <memory>

class PeraPipeline : public PipelineObject{
protected:
	struct WvpMat {
		Mat4x4 worldMat;
		Mat4x4 viewProjection;
	};

public:
	PeraPipeline() = default;
	PeraPipeline(const PeraPipeline&) = delete;
	PeraPipeline(PeraPipeline&&) = delete;
	virtual ~PeraPipeline();

	PeraPipeline& operator=(const PeraPipeline&) = delete;
	PeraPipeline& operator=(PeraPipeline&&) = delete;

public:
	virtual void Use(Pipeline::Blend blendType, bool isDepth = false) override;

	virtual void Init(
		const std::string& vsShader,
		const std::string& psShader,
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	virtual void Update();

	void SetSize(uint32_t width, uint32_t height) {
		width_ = width;
		height_ = height;
	}

	RenderTarget& GetRender() {
		return *render_;
	}

	const RenderTarget& GetRender() const {
		return *render_;
	}

public:
	Mat4x4 worldMat = Mat4x4::kIdentity;
	Mat4x4 viewProjection = Mat4x4::kIdentity;
	Vector4 color = Vector4::kIdentity;

protected:
	std::unique_ptr<RenderTarget> render_;

	ConstBuffer<WvpMat> wvpMat_;
	ConstBuffer<Vector4> colorBuf_;
	uint32_t width_ = 0u;
	uint32_t height_ = 0u;
};