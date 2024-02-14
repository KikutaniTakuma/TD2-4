#pragma once
#include "Pipeline/Pipeline.h"
#include <list>
#include <vector>
#include <memory>
#include <tuple>
class RootSignature;

/// <summary>
/// グラフィックスパイプラインとルートシグネチャの管理
/// </summary>
class PipelineManager {
/// <summary>
/// コンストラクタ
/// </summary>
private:
	PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager(PipelineManager&&) = delete;

	PipelineManager& operator=(const PipelineManager&) = delete;
	PipelineManager& operator=(PipelineManager&&) = delete;
public:
	~PipelineManager() = default;

/// <summary>
/// 静的関数
/// </summary>
public:
	static void Initialize();
	static void Finalize();

	///
	/// 下の4つの関数を使った後にCreateする
	/// 
	
	/// <summary>
	/// ルートシグネチャを生成(この関数を呼び出したらそのルートシグネチャがPSOに使われる)
	/// </summary>
	/// <param name="rootParamater_">ルートパラメータ</param>
	/// <param name="isTexture_">テクスチャを使う場合はtrue</param>
	static void CreateRootSgnature(D3D12_ROOT_PARAMETER* rootParamater, size_t rootParamaterSize, bool isTexture, bool isOutRangeBorder = false);

	static void SetRootSgnature(RootSignature* rootSignature);

	/// <summary>
	/// 頂点シェーダの入力設定(呼び出し毎に複数設定可能)
	/// </summary>
	/// <param name="semanticName_">セマンティクス名</param>
	/// <param name="semanticIndex_">セマンティクスインデックス</param>
	/// <param name="format_">フォーマット</param>
	static void SetVertexInput(std::string semanticName, uint32_t semanticIndex, DXGI_FORMAT format);

	/// <summary>
	/// 使うシェーダの設定
	/// </summary>
	/// <param name="shader_">シェーダ</param>
	static void SetShader(const Shader& shader);

	/// <summary>
	/// ステイタスの設定
	/// </summary>
	/// <param name="blend_">ブレンドの設定</param>
	/// <param name="solidState_">ソリッド設定</param>
	/// <param name="cullMode_">カリング設定</param>
	/// <param name="numRenderTarget_">レンダーターゲットの数</param>
	static void SetState(
		Pipeline::Blend blend,
		Pipeline::SolidState solidState,
		Pipeline::CullMode cullMode = Pipeline::CullMode::Back,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		uint32_t numRenderTarget = 1u
	);

	static void IsDepth(bool isDepth = true);

	/// <summary>
	/// 設定したものでPSOの生成
	/// </summary>
	/// <returns>psoのポインタ(勝手にdeleteしてはいけない)</returns>
	static Pipeline* const Create();

	/// <summary>
	/// CreateRootSgnature(), SetVertexInput(), SetShader(), SetState()で設定した値をリセット
	/// </summary>
	static void StateReset();

/// <summary>
/// シングルトンインスタンス
/// </summary>
private:
	static PipelineManager* instance_;

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::list<std::unique_ptr<Pipeline>> pipelines_;
	std::list<std::unique_ptr<RootSignature>> rootSignatures_;

	RootSignature* rootSignature_;

	Shader shader_;
	Pipeline::Blend blend_;
	Pipeline::CullMode cullMode_;
	Pipeline::SolidState solidState_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_;
	uint32_t numRenderTarget_;
	bool isDepth_;

	std::vector<std::tuple<std::string, uint32_t, DXGI_FORMAT>> vertexInputStates_;
};