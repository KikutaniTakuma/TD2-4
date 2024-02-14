#pragma once
#include <string>
#include <unordered_map>
#include <initializer_list>

#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"

#include "Engine/Buffer/StructuredBuffer/StructuredBuffer.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"

/// <summary>
/// メッシュクラス。objファイルの読み込みやリソースを管理(基本的にポインタ型で使う)
/// </summary>
class Mesh {
	friend class MeshManager;

public:
	struct ResourceData {
		std::pair<Lamb::LambPtr<ID3D12Resource>, D3D12_VERTEX_BUFFER_VIEW> resource;
		uint32_t vertNum;
		Texture* tex;
	};

	struct CopyData {
		D3D12_VERTEX_BUFFER_VIEW view;
		uint32_t vertNum;
		Texture* tex;

		CopyData& operator=(const ResourceData& other) {
			view = other.resource.second;
			vertNum = other.vertNum;
			tex = other.tex;

			return *this;
		}
	};

	struct MatrixData {
		Mat4x4 worldMat;
		Mat4x4 viewProjectoionMat;
	};

	struct Light {
		Vector3 ligDirection;
		float pad0;
		Vector3 ligColor;
		float pad1;
		Vector3 eyePos;
		float pad2;
		Vector3 ptPos;
		float pad3;
		Vector3 ptColor;
		float ptRange;
	};

private:
	struct VertData {
		Vector4 position;
		Vector3 normal;
		Vector2 uv;
	};
	struct IndexData {
		uint32_t vertNum;
		uint32_t uvNum;
		uint32_t normalNum;

		inline bool operator==(const IndexData& right) {
			return vertNum == right.vertNum
				&& uvNum == right.uvNum
				&& normalNum == right.normalNum;
		}
		inline bool operator!=(const IndexData& right) {
			return !(*this == right);
		}
	};

	struct VertResourece {
		Lamb::LambPtr<ID3D12Resource> vertexBuffer = nullptr;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		// 頂点バッファマップ
		VertData* vertexMap = nullptr;
	};

	struct MeshData{
		std::unordered_map<size_t, VertData> vertices;

		UINT sizeInBytes;
		UINT strideInBytes;

		// 頂点数
		uint32_t vertNum = 0;
	};
	
public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	~Mesh();

public:
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) = delete;

public:
	std::unordered_map<std::string, Mesh::CopyData> CopyBuffer() const;

	inline bool GetIsLoad() const {
		return isLoad_;
	}

	inline const std::string& GetFileName()const {
		return objFileName_;
	}

private:
	void LoadObj(const std::string& objFileName);
	void LoadMtl(const std::string& fileName);

	/// <summary>
	/// 非同期読み込み用(この関数単体では非同期では読み込まない)
	/// </summary>
	/// <param name="objFileName"></param>
	void ThreadLoadObj(const std::string& objFileName);
	/// <summary>
	/// 非同期読み込み用
	/// </summary>
	/// <param name="fileName"></param>
	void ThreadLoadMtl(const std::string& fileName);

	/// <summary>
	/// テクスチャをロード出来ているかのチェック
	/// </summary>
	void CheckModelTextureLoadFinish();

	/// <summary>
	/// 頂点リソースを作成
	/// </summary>
	void CreateResource();

	/// <summary>
	/// 頂点リソースを解放
	/// </summary>
	void ReleaseResource();

public:
	void ResetDrawCount();

	void Use(
		const Mat4x4& wvpMat,
		const Mat4x4& viewOrojection,
		const Light& light,
		const Vector4& color
		);

	void Draw();

	void ChangeTexture(const std::string& useMtlName, const std::string& texName);
	void ChangeTexture(const std::string& useMtlName, Texture* tex);

private:
	void ResizeBuffers();
	void ResetBufferSize();

private:
	std::unordered_map<std::string, MeshData> meshs_;

	std::unordered_map<std::string, Texture*> texs_;

	bool isLoad_;
	bool isObjLoad_;

	StructuredBuffer<MatrixData> wvpMats_;
	ConstBuffer<Light> dirLig_;
	ConstBuffer<Vector4> color_;

	uint32_t drawCount_;

	std::unordered_map<std::string, Mesh::ResourceData> resource_;

	std::string objFileName_;

public:
	static void Initialize(
		const std::string& vertex = "./Resources/Shaders/ModelInstancingShader/ModelInstancing.VS.hlsl",
		const std::string& pixel = "./Resources/Shaders/ModelInstancingShader/ModelInstancing.PS.hlsl",
		const std::string& geometory = {},
		const std::string& hull = {},
		const std::string& domain = {}
	);

private:
	static void LoadShader(
		const std::string& vertex,
		const std::string& pixel,
		const std::string& geometory,
		const std::string& hull,
		const std::string& domain
	);
	static void CreateGraphicsPipeline();

private:
	static Shader shader_;

	static class Pipeline* pipeline_;

	static bool loadShaderFlg_;
	static bool createGPFlg_;
};