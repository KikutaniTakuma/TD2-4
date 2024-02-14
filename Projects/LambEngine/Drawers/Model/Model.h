#pragma once
#include <string>
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include <unordered_map>
#include "Engine/Graphics/MeshManager/MeshManager.h"

class Pipeline;

#include <wrl.h>

/// <summary>
/// 3Dモデルの描画
/// </summary>
class Model {
public:
	Model();
	Model(const std::string& fileName);
	Model(const Model& right);
	Model(Model&& right) noexcept;
	~Model();

	Model& operator=(const Model& right);
	Model& operator=(Model&& right) noexcept;

public:
	void LoadObj(const std::string& fileName);

	void ThreadLoadObj(const std::string& fileName);

public:
	static void Initialize(
		const std::string& vertex = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
		const std::string& pixel = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
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

	static Pipeline* defaultPipeline_;
	static bool loadShaderFlg_;
	static bool createGPFlg_;


public:
	void Update();

	void Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);

	void InstancingDraw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);


	void Debug(const std::string& guiName);

	void SetParent(Model* parent) {
		parent_ = parent;
	}
	
	void ChangeTexture(const std::string& useMtlName, const std::string& texName);
	void ChangeTexture(const std::string& useMtlName, Texture* tex);

	void MeshChangeTexture(const std::string& useMtlName, const std::string& texName);
	void MeshChangeTexture(const std::string& useMtlName, Texture* tex);

	void ChangeMesh(Mesh* mesh) {
		mesh_ = mesh;
		isLoadObj_ = !!mesh_;
	}

	const std::string GetObjFileName() const {
		if (mesh_) {
			return mesh_->GetFileName();
		}
		else {
			return std::string{};
		}
	}

	const Mat4x4& GetWorldMatrix() const {
		return wvpData_->worldMat;
	}

	void SetIsLighting(bool isLighting) {
		*isLighting_ = static_cast<uint32_t>(isLighting);
	}

	void SetPipeline(Pipeline* const pipeline);

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

	uint32_t color;

	Mesh::Light light;

private:
	Model* parent_;
	Mesh* mesh_;

	Pipeline* pipeline_;

	std::unordered_map<std::string, Mesh::CopyData> data_;

	bool isLoadObj_;

	ConstBuffer<Mesh::MatrixData> wvpData_;

	ConstBuffer<Mesh::Light> dirLig_;

	ConstBuffer<Vector4> colorBuf_;
	ConstBuffer<uint32_t> isLighting_;
};