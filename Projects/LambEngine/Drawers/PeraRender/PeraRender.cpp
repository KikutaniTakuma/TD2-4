#include "PeraRender.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "imgui.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include <cassert>
#include <numbers>

#include "Utils/Random/Random.h"

#include "Utils/EngineInfo/EngineInfo.h"

#include "Utils/SafePtr/SafePtr.h"

PeraRender::PeraRender():
	peraVertexResource_(nullptr),
	peraVertexView_(),
	indexView_{},
	indexResource_{nullptr},
	pos{},
	rotate{},
	scale{Lamb::ClientSize()},
	isPreDraw_(false),
	uvPibot(),
	uvSize(Vector2::kIdentity),
	color(std::numeric_limits<uint32_t>::max())
{
	peraPipelineObject_.reset(new PeraPipeline{});
}

PeraRender::PeraRender(uint32_t width, uint32_t height):
	peraVertexResource_(nullptr),
	peraVertexView_(),
	indexView_{},
	indexResource_{ nullptr },
	pos{},
	rotate{},
	scale{ Vector3::kIdentity },
	isPreDraw_(false),
	uvPibot(),
	uvSize(Vector2::kIdentity),
	color(std::numeric_limits<uint32_t>::max())
{
	peraPipelineObject_.reset(new PeraPipeline{});

	peraPipelineObject_->SetSize(width, height);
}

PeraRender::~PeraRender() {
	peraPipelineObject_.reset();
}

void PeraRender::Initialize(const std::string& psFileName) {
	peraPipelineObject_->Init("./Resources/Shaders/PostShader/Post.VS.hlsl", psFileName);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(indices));
	indexResource_.SetName<PeraRender>();
	indexView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexView_.SizeInBytes = sizeof(indices);
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	Lamb::SafePtr<uint16_t> indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource_->Unmap(0, nullptr);

	std::array<PeraVertexData, 4> pv = {
		Vector3{ -0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 1.0f },
		Vector3{  0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 0.0f },
		Vector3{  0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 1.0f },
		Vector3{ -0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 0.0f }
	};

	peraVertexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(pv));
	peraVertexResource_.SetName<PeraRender>();

	peraVertexView_.BufferLocation = peraVertexResource_->GetGPUVirtualAddress();
	peraVertexView_.SizeInBytes = sizeof(pv);
	peraVertexView_.StrideInBytes = sizeof(PeraVertexData);

	Lamb::SafePtr<PeraVertexData> mappedData = nullptr;
	peraVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource_->Unmap(0, nullptr);
}

void PeraRender::Initialize(PeraPipeline* pipelineObject) {
	ResetPipelineObject(pipelineObject);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(indices));
	indexResource_.SetName<PeraRender>();
	indexView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexView_.SizeInBytes = sizeof(indices);
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	Lamb::SafePtr<uint16_t> indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource_->Unmap(0, nullptr);

	std::array<PeraVertexData, 4> pv = {
		Vector3{ -0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 1.0f },
		Vector3{  0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 0.0f },
		Vector3{  0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 1.0f },
		Vector3{ -0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 0.0f }
	};

	peraVertexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(pv));
	peraVertexResource_.SetName<PeraRender>();

	peraVertexView_.BufferLocation = peraVertexResource_->GetGPUVirtualAddress();
	peraVertexView_.SizeInBytes = sizeof(pv);
	peraVertexView_.StrideInBytes = sizeof(PeraVertexData);

	Lamb::SafePtr<PeraVertexData> mappedData = nullptr;
	peraVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource_->Unmap(0, nullptr);
}

void PeraRender::Update() {
	isPreDraw_ = false;

	peraPipelineObject_->color = UintToVector4(color);
}

void PeraRender::PreDraw() {
	peraPipelineObject_->GetRender().SetThisRenderTarget();
	isPreDraw_ = true;
}

void PeraRender::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend, 
	PeraRender* pera, 
	bool isDepth
) {
	if (!!pera) {
		pera->PreDraw();
		peraPipelineObject_->GetRender().ChangeResourceState();
	}
	else {
		// 描画先をメインレンダーターゲットに変更
		peraPipelineObject_->GetRender().SetMainRenderTarget();
	}

	const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
	const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

	const std::array<Vector3, 4> localpos{
			Vector3{ -0.5f,  0.5f, 0.0f },
			Vector3{  0.5f,  0.5f, 0.0f },
			Vector3{  0.5f, -0.5f, 0.0f },
			Vector3{ -0.5f, -0.5f, 0.0f },
	};

	std::array<PeraVertexData, 4> pv = {
		localpos[0], uv3,
		localpos[1], uv2,
		localpos[2], uv1,
		localpos[3], uv0,
	};

	Lamb::SafePtr<PeraVertexData> mappedData = nullptr;
	peraVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource_->Unmap(0, nullptr);

	peraPipelineObject_->worldMat = Mat4x4::MakeAffin(
		Vector3(scale.x, scale.y, 1.0f),
		rotate,
		pos
	);
	peraPipelineObject_->viewProjection = viewProjection;

	peraPipelineObject_->Update();

	// 各種描画コマンドを積む
	Lamb::SafePtr commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	peraPipelineObject_->Use(blend, isDepth);
	commandList->IASetVertexBuffers(0, 1, &peraVertexView_);
	commandList->IASetIndexBuffer(&indexView_);
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void PeraRender::ResetPipelineObject(PeraPipeline* pipelineObject) {
	if (!pipelineObject) {
		throw Lamb::Error::Code<PeraRender>("pipelineObject is nullptr", __func__);
	}
	else {
		peraPipelineObject_.reset(pipelineObject);
	}
}

void PeraRender::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::ColorEdit4("color", peraPipelineObject_->color.m.data());

	color = Vector4ToUint(peraPipelineObject_->color);
	ImGui::End();
#endif // _DEBUG
}