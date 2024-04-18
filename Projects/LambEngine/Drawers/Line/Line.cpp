#include "Line.h"
#include <algorithm>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Error/Error.h"
#include "imgui.h"
#include <numbers>

uint32_t Line::indexCount_ = 0u;
Shader Line::shader_ = {};
Lamb::SafePtr<Pipeline> Line::pipline_ = nullptr;
Lamb::LambPtr<ID3D12Resource> Line::vertexBuffer_;
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Line::vertexView_;

std::unique_ptr<StructuredBuffer<Line::VertxData, Line::kDrawMaxNumber_>> Line::vertData_;

void Line::Initialize() {
	Lamb::SafePtr shaderManager = ShaderManager::GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader("./Resources/Shaders/LineShader/Line.VS.hlsl");
	shader_.pixel = shaderManager->LoadPixelShader("./Resources/Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER paramater = {};
	paramater.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	paramater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramater.DescriptorTable.pDescriptorRanges = &range;
	paramater.DescriptorTable.NumDescriptorRanges = 1;

	PipelineManager::CreateRootSgnature(&paramater, 1, false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipline_ = PipelineManager::Create();
	PipelineManager::StateReset();

	vertData_ = std::make_unique<StructuredBuffer<VertxData, kDrawMaxNumber_>>();

	Lamb::SafePtr heap = CbvSrvUavHeap::GetInstance();
	heap->BookingHeapPos(1);
	heap->CreateView(*vertData_);

	vertexBuffer_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(Vector4) * kVertexNum);
	vertexView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(Vector4) * kVertexNum;
	vertexView_.StrideInBytes = sizeof(Vector4);

	Lamb::SafePtr<Vector4> vertexMap = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));
	vertexMap[0] = Vector4::kZero + Vector4::kWIdentity;
	vertexMap[1] = Vector4::kXIdentity + Vector4::kWIdentity;
	vertexBuffer_->Unmap(0, nullptr);

	vertexBuffer_.SetName<Line>();
}

void Line::Finalize() {
	if (vertData_) {
		CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
		heap->ReleaseView(vertData_->GetHandleUINT());
		vertData_.reset();
	}

	if (vertexBuffer_) {
		vertexBuffer_.Reset();
	}
}

void Line::ResetDrawCount() {
	indexCount_ = 0u;
}

void Line::AllDraw() {
	if (indexCount_ == 0u) {
		return;
	}

	pipline_->Use();
	Lamb::SafePtr heap = CbvSrvUavHeap::GetInstance();
	heap->Use(vertData_->GetHandleUINT(), 0);
	auto commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &vertexView_);
	commandList->DrawInstanced(kVertexNum, indexCount_, 0, 0);

	ResetDrawCount();
}

Line::Line():
	start{},
	end{},
	color(std::numeric_limits<uint32_t>::max())
{}

void Line::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("start", &start.x, 0.01f);
	ImGui::DragFloat3("end", &end.x, 0.01f);
	ImGui::End();
#endif // _DEBUG
}

void Line::Draw(const Mat4x4& viewProjection) {
	assert(indexCount_ < kDrawMaxNumber_);
	if (!(indexCount_ < kDrawMaxNumber_)) {
		Lamb::Error::Code<Line>("Over Draw index", __func__);
	}

	auto&& colorFloat = UintToVector4(color);

	(*vertData_)[indexCount_].color = colorFloat;

	Vector3 scale;
	scale.x = (end - start).Length();
	Vector3 to = (end - start).Normalize();
	Vector3 translate = start;

	(*vertData_)[indexCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

	indexCount_++;
}

void Line::Draw(const Vector3& start, const Vector3& end, const Mat4x4& viewProjection, uint32_t color = std::numeric_limits<uint32_t>::max()) {
	assert(indexCount_ < kDrawMaxNumber_);
	if (!(indexCount_ < kDrawMaxNumber_)) {
		throw Lamb::Error::Code<Line>("Over Draw index", __func__);
	}

	auto&& colorFloat = UintToVector4(color);

	(*vertData_)[indexCount_].color = colorFloat;

	Vector3 scale;
	scale.x = (end - start).Length();
	Vector3 to = (end - start).Normalize();
	Vector3 translate = start;

	(*vertData_)[indexCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

	indexCount_++;
}