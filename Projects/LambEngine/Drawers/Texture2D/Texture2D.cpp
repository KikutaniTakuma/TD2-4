#include "Texture2D.h"
#include "imgui.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include <numeric>
#undef max
#undef min
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Error/Error.h"

/// <summary>
/// 静的変数のインスタンス化
/// </summary>

std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Texture2D::graphicsPipelineState_ = {};
std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Texture2D::graphicsPipelineStateNoDepth_ = {};
std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Texture2D::graphicsPipelineStateNoWrap_ = {};
std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Texture2D::graphicsPipelineStateNoWrapNoDepth_ = {};
Shader Texture2D::shader_ = {};

D3D12_INDEX_BUFFER_VIEW Texture2D::indexView_ = {};
Lamb::LambPtr<ID3D12Resource> Texture2D::indexResource_ = nullptr;

Texture2D::Texture2D() :
	scale(Vector2::kIdentity),
	rotate(),
	pos({ 0.0f,0.0f,0.01f }),
	uvPibot(),
	uvSize(Vector2::kIdentity),
	tex_(nullptr),
	isLoad_(false),
	color(std::numeric_limits<uint32_t>::max()),
	wvpMat_(),
	colorBuf_(),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd(0.0f),
	isAnimation_(0.0f),
	isSameTexSize(),
	texScalar(1.0f)
{
	*wvpMat_ = Mat4x4::kIdentity;
	*colorBuf_ = Vector4::kIdentity;

	vertexResource_ =DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * 4);
	vertexResource_.SetName<Texture2D>();

	vertexView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexView_.StrideInBytes = sizeof(VertexData);

	tex_ = TextureManager::GetInstance()->GetWhiteTex();

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}

	auto srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(2u);
	srvHeap->CreateView(wvpMat_);
	srvHeap->CreateView(colorBuf_);
}

Texture2D::Texture2D(const std::string& fileName):
	Texture2D()
{
	this->LoadTexture(fileName);
}

Texture2D::Texture2D(const Texture2D& right) :
	Texture2D()
{
	*this = right;
}
Texture2D::Texture2D(Texture2D&& right) noexcept :
	Texture2D()
{
	*this = std::move(right);
}

Texture2D& Texture2D::operator=(const Texture2D& right) {
	scale = right.scale;
	rotate = right.rotate;
	pos = right.pos;

	uvPibot = right.uvPibot;
	uvSize = right.uvSize;

	color = right.color;

	worldPos = right.worldPos;

	tex_ = right.tex_;
	isLoad_ = right.isLoad_;


	*wvpMat_ = *right.wvpMat_;
	*colorBuf_ = *right.colorBuf_;

	aniStartTime_ = right.aniStartTime_;
	aniCount_ = right.aniCount_;
	isAnimation_ = right.isAnimation_;
	uvPibotSpd = right.uvPibotSpd;

	isSameTexSize = right.isSameTexSize;

	texScalar = right.texScalar;

	return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& right) noexcept {
	scale = std::move(right.scale);
	rotate = std::move(right.rotate);
	pos = std::move(right.pos);

	uvPibot = std::move(right.uvPibot);
	uvSize = std::move(right.uvSize);

	color = std::move(right.color);

	worldPos = std::move(right.worldPos);

	tex_ = std::move(right.tex_);
	isLoad_ = std::move(right.isLoad_);

	*wvpMat_ = *right.wvpMat_;
	*colorBuf_ = *right.colorBuf_;

	aniStartTime_ = std::move(right.aniStartTime_);
	aniCount_ = std::move(right.aniCount_);
	isAnimation_ = std::move(right.isAnimation_);
	uvPibotSpd = std::move(right.uvPibotSpd);

	isSameTexSize = std::move(right.isSameTexSize);
	texScalar = std::move(right.texScalar);

	return *this;
}

Texture2D::~Texture2D() {
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->ReleaseView(wvpMat_.GetHandleUINT());
	descriptorHeap->ReleaseView(colorBuf_.GetHandleUINT());
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	LoadShader(vsFileName, psFileName);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(indices));
	indexResource_.SetName<Texture2D>();

	indexView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexView_.SizeInBytes = sizeof(indices);
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource_->Unmap(0, nullptr);

	CreateGraphicsPipeline();
}

void Texture2D::Finalize() {
	indexResource_.Reset();
}

void Texture2D::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	static ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader(vsFileName);
	assert(shader_.vertex);
	shader_.pixel = shaderManager->LoadPixelShader(psFileName);
	assert(shader_.pixel);
}

void Texture2D::CreateGraphicsPipeline() {
	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].NumDescriptors = 1;
	texRange[0].BaseShaderRegister = 0;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	std::array<D3D12_ROOT_PARAMETER, 2> rootPrams{};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(cbvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true, false);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	for (int32_t i = Pipeline::Blend::None; i < graphicsPipelineState_.size(); i++) {
		PipelineManager::IsDepth(true);
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineState_[i] = PipelineManager::Create();

		PipelineManager::IsDepth(false);
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineStateNoDepth_[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();

	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true, true);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	for (int32_t i = Pipeline::Blend::None; i < graphicsPipelineStateNoWrap_.size(); i++) {
		PipelineManager::IsDepth(true);
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineStateNoWrap_[i] = PipelineManager::Create();

		PipelineManager::IsDepth(false);
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineStateNoWrapNoDepth_[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();

	for (auto& i : graphicsPipelineState_) {
		if (!i) {
			throw Lamb::Error::Code<Texture2D>("pipeline is nullptr", __func__);
		}
	}
}

void Texture2D::LoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	while (textureManager->IsNowThreadLoading()) {
		// 非同期読み込みが終わるまでビジーループ
	}
	tex_ = textureManager->LoadTexture(fileName);

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}
}

void Texture2D::ThreadLoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	tex_ = nullptr;
	textureManager->LoadTexture(fileName, &tex_);
	isLoad_ = false;
}

void Texture2D::Update() {

	if (tex_ && tex_->CanUse() && !isLoad_) {
		isLoad_ = true;
	}

	if (tex_ && isLoad_) {
		if (isSameTexSize) {
			scale = tex_->getSize() * texScalar;
		}
		else if(isSameTexSize.OnExit()) {
			scale.x /= tex_->getSize().x;
			scale.y /= tex_->getSize().y;
		}

		static const std::array<Vector3, 4> pv{
			Vector3{ -0.5f,  0.5f, 0.1f },
			Vector3{  0.5f,  0.5f, 0.1f },
			Vector3{  0.5f, -0.5f, 0.1f },
			Vector3{ -0.5f, -0.5f, 0.1f },
		};

		std::copy(pv.begin(), pv.end(), worldPos.begin());
		auto&& worldMat =
			Mat4x4::MakeAffin(
				Vector3(scale.x, scale.y, 1.0f),
				rotate,
				pos
			);
		for (auto& i : worldPos) {
			i *= worldMat;
		}

		*colorBuf_ = UintToVector4(color);
	}
}

void Texture2D::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend,
	bool isDepth,
	bool isWrap
) {
	if (tex_ && isLoad_) {
		const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
		const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

		std::array<VertexData, 4> pv = {
			worldPos[0], uv3,
			worldPos[1], uv2,
			worldPos[2], uv1,
			worldPos[3], uv0,
		};

		VertexData* mappedData = nullptr;
		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource_->Unmap(0, nullptr);

		*wvpMat_ = viewProjection;

		auto commandlist = DirectXCommand::GetInstance()->GetCommandList();


		// 各種描画コマンドを積む
		if (!isDepth && isWrap) {
			graphicsPipelineStateNoDepth_[blend]->Use();
		}
		else if (isDepth && !isWrap) {
			graphicsPipelineStateNoWrap_[blend]->Use();
		}
		else if (!isDepth && !isWrap) {
			graphicsPipelineStateNoWrapNoDepth_[blend]->Use();
		}
		else {
			graphicsPipelineState_[blend]->Use();
		}
		
		tex_->Use(0);
		commandlist->SetGraphicsRootDescriptorTable(1, wvpMat_.GetHandleGPU());
		commandlist->IASetVertexBuffers(0, 1, &vertexView_);
		commandlist->IASetIndexBuffer(&indexView_);
		commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture2D::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	*colorBuf_ = UintToVector4(color);
	ImGui::Begin(guiName.c_str());
	ImGui::Checkbox("is same scale and Texture", isSameTexSize.Data());
	if (isSameTexSize) {
		ImGui::DragFloat("tex scalar", &texScalar, 0.01f);
	}
	ImGui::DragFloat2("scale", &scale.x, 1.0f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("pos", &pos.x, 1.0f);
	ImGui::DragFloat2("uvPibot", &uvPibot.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf_->color.r);
	color = Vector4ToUint(*colorBuf_);

	if (ImGui::TreeNode("tex load")) {
		if (isLoad_) {
		auto texures = Lamb::GetFilePathFormDir("./Resources/", ".png");

			for (auto& i : texures) {
				if (ImGui::Button(i.string().c_str())) {
					this->ThreadLoadTexture(i.string());
					break;
				}
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}

bool Texture2D::Collision(const Vector2& pos2D) const {
	Vector2 max;
	Vector2 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;

	if (min.x < pos2D.x && pos2D.x < max.x) {
		if (min.y < pos2D.y && pos2D.y < max.y) {
			return true;
		}
	}

	return false;
}

bool Texture2D::Collision(const Texture2D& tex2D) const {
	Vector3 max;
	Vector3 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max.z = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.z = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;

	// 追加変更。by Korone
	Vector3 max2;
	Vector3 min2;
	max2.x = std::max_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max2.y = std::max_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max2.z = std::max_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min2.x = std::min_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min2.y = std::min_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min2.z = std::min_element(tex2D.worldPos.begin(), tex2D.worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;

	if (min.x <= max2.x && max.x >= min2.x &&
		min.y <= max2.y && max.y >= min2.y) {
		return true;
	}

	/*for (auto& i : tex2D.worldPos) {
		if (min.x <= i.x && i.x <= max.x) {
			if (min.y <= i.y && i.y <= max.y) {
				return true;
			}
		}
	}*/

	return false;
}

void Texture2D::ChangeTexture(Texture* tex) {
	tex_ = tex;
	isLoad_ = !!tex_;
}

void Texture2D::AnimationStart(float aniUvPibot) {
	if (!isAnimation_) {
		aniStartTime_ = std::chrono::steady_clock::now();
		isAnimation_ = true;
		aniCount_ = 0.0f;
		uvPibot.x = aniUvPibot;
	}
}

void Texture2D::AnimationPause() {
	isAnimation_ = false;
}

void Texture2D::AnimationRestart() {
	isAnimation_ = true;
}

void Texture2D::Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd) {
	if (isAnimation_) {
		auto nowTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - aniStartTime_) >= std::chrono::milliseconds(aniSpd)) {
			aniStartTime_ = nowTime;
			aniCount_++;

			if (aniCount_ >= aniUvEnd) {
				if (isLoop) {
					aniCount_ = aniUvStart;
				}
				else {
					--aniCount_;
					isAnimation_ = false;
				}
			}

			uvPibot.x = aniUvStart;
			uvPibot.x += uvPibotSpd * aniCount_;
		}
	}
}