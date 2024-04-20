#pragma once
#include <string>
#include <array>

#include "Engine/Graphics/PipelineObject/PeraPipeline/PeraPipeline.h"

#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Mat4x4.h"

/// <summary>
/// ポストエフェクトの描画
/// </summary>
class PeraRender {
public:
	struct PeraVertexData {
		Vector3 position;
		Vector2 uv;
	};

public:
	PeraRender();
	PeraRender(uint32_t width, uint32_t height);
	~PeraRender();

	PeraRender(const PeraRender&) = delete;
	PeraRender(PeraRender&&) = delete;
	PeraRender& operator=(const PeraRender&) = delete;
	PeraRender& operator=(PeraRender&&) = delete;

public:
	void Initialize(const std::string& psFileName);
	void Initialize(PeraPipeline* pipelineObject);

public:
	void Update();

	void PreDraw();

	void Draw(
		const Mat4x4& viewProjection, 
		Pipeline::Blend blend, 
		PeraRender* pera = nullptr,
		bool isDepth = false
	);

	Texture* GetTex() const {
		return peraPipelineObject_->GetRender().GetTex();
	}

	void ChangeResourceState() {
		peraPipelineObject_->GetRender().ChangeResourceState();
	}

	void SetMainRenderTarget() {
		peraPipelineObject_->GetRender().SetMainRenderTarget();
	}

	void Debug(const std::string& guiName);

	void ResetPipelineObject(PeraPipeline* pipelineObject);

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

	Vector2 uvPibot;
	Vector2 uvSize;

	uint32_t color;

private:
	std::unique_ptr<PeraPipeline> peraPipelineObject_;

	bool isPreDraw_;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView_;
	Lamb::LambPtr<ID3D12Resource> peraVertexResource_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexView_;
	Lamb::LambPtr<ID3D12Resource> indexResource_;
};