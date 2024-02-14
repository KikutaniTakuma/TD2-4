#include "Pipeline.h"
#include <cassert>
#include <algorithm>
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Graphics/RootSignature/RootSignature.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Error/Error.h"

Pipeline::Pipeline():
	graphicsPipelineState_(),
	shader_(),
	vertexInput_(0),
	blend_(),
	cullMode_(),
	solidState_(),
	numRenderTarget_(1u),
	semanticNames_(0),
	topologyType_(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE),
	rootSignature_(nullptr),
	isDepth_(true)
{
	vertexInput_.reserve(0);
}

Pipeline::Pipeline(Pipeline&& right) noexcept {
	*this = std::move(right);
}

Pipeline& Pipeline::operator=(Pipeline&& right) noexcept {
	graphicsPipelineState_ = std::move(right.graphicsPipelineState_);
	shader_ = std::move(right.shader_);

	return *this;
}

bool Pipeline::operator==(const Pipeline& right) const {
	return shader_.vertex == right.shader_.vertex
		&& shader_.pixel == right.shader_.pixel
		&& shader_.hull == right.shader_.hull
		&& shader_.domain == right.shader_.domain
		&& shader_.geometory == right.shader_.geometory
		&& blend_ == right.blend_
		&& cullMode_ == right.cullMode_
		&& solidState_ == right.solidState_
		&& numRenderTarget_ && right.numRenderTarget_
		&& isDepth_ == right.isDepth_;
}
bool Pipeline::operator!=(const Pipeline& right) const {
	return !this->operator==(right);
}

void Pipeline::SetVertexInput(std::string semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {
	D3D12_INPUT_ELEMENT_DESC inputElementDescs{};

	inputElementDescs.SemanticIndex = semanticIndex;
	inputElementDescs.Format = format;
	inputElementDescs.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

	vertexInput_.push_back(inputElementDescs);
	semanticNames_.push_back(semanticName);
}

void Pipeline::SetShader(const Shader& shader) {
	shader_ = shader;

	assert(shader_.hull == shader_.domain || shader_.hull != nullptr && shader_.domain != nullptr);
}

void Pipeline::Create(
	const RootSignature& rootSignature,
	Pipeline::Blend blend,
	Pipeline::CullMode cullMode,
	Pipeline::SolidState solidState,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType,
	uint32_t numRenderTarget,
	bool isDepth
) {
	blend_ = blend;
	cullMode_ = cullMode;
	solidState_ = solidState;
	topologyType_ = topologyType;
	numRenderTarget_ = numRenderTarget;
	isDepth_ = isDepth;

	rootSignature_ = rootSignature.Get();


	numRenderTarget_ = std::clamp(numRenderTarget_, 1u, 8u);

	for (size_t i = 0; i < vertexInput_.size(); i++) {
		vertexInput_[i].SemanticName = semanticNames_[i].c_str();
	}

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = vertexInput_.data();
	inputLayoutDesc.NumElements = UINT(vertexInput_.size());

	// BlendStateの設定
	D3D12_BLEND_DESC blendDec{};
	// 全ての色要素を書き込む
	blendDec.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE(cullMode_);
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE(solidState_);
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature_;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			shader_.vertex->GetBufferPointer(),
			shader_.vertex->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			shader_.pixel->GetBufferPointer(),
			shader_.pixel->GetBufferSize()
	};
	if (shader_.hull && shader_.domain) {
		graphicsPipelineStateDesc.HS = {
				shader_.hull->GetBufferPointer(),
				shader_.hull->GetBufferSize()
		};
		graphicsPipelineStateDesc.DS = {
				shader_.domain->GetBufferPointer(),
				shader_.domain->GetBufferSize()
		};
	}
	if (shader_.geometory) {
		graphicsPipelineStateDesc.GS = {
				shader_.geometory->GetBufferPointer(),
				shader_.geometory->GetBufferSize()
		};
	}


	graphicsPipelineStateDesc.BlendState = blendDec;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = numRenderTarget_;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = topologyType_;

	// どのように画面に打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	if (isDepth_) {
		graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	}

	for (uint32_t i = 0; i < numRenderTarget_; i++) {
		switch (blend_)
		{
		case Pipeline::Blend::None:
		default:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = false;
			break;
		case Pipeline::Blend::Normal:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Add:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Sub:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_SUBTRACT;
			break;
		case Pipeline::Blend::Mul:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_SRC_COLOR;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		}

		graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Pipeline>("CreateGraphicsPipelineState failed", __func__);
	}
}

void Pipeline::Use() const {
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	assert(graphicsPipelineState_);
	if (!graphicsPipelineState_) {
		throw Lamb::Error::Code<Pipeline>("GraphicsPipelineState is nullptr", __func__);
	}
	auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootSignature(rootSignature_);
	commandlist->SetPipelineState(graphicsPipelineState_.Get());

	switch (topologyType_)
	{
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		break;
	default:
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED:
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
		throw Lamb::Error::Code<Pipeline>("Cannot use this primitive topology type", __func__);
		break;
	}
}

bool Pipeline::IsSame(
	const Shader& shader,
	Pipeline::Blend blend,
	Pipeline::CullMode cullMode,
	Pipeline::SolidState solidState,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType,
	uint32_t numRenderTarget,
	ID3D12RootSignature* rootSignature,
	bool isDepth
) {
	return shader_.vertex == shader.vertex
		&& shader_.pixel == shader.pixel
		&& shader_.hull == shader.hull
		&& shader_.domain == shader.domain
		&& shader_.geometory == shader.geometory
		&& blend_ == blend
		&& cullMode_ == cullMode
		&& solidState_ == solidState
		&& topologyType_ == topologyType
		&& numRenderTarget_ == numRenderTarget
		&& rootSignature_ == rootSignature
		&& isDepth_ == isDepth;
}