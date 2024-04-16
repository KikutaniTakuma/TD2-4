//#pragma once
//#include <memory>
//#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
//#include "Engine/Graphics/MeshManager/MeshManager.h"
//#include "Engine/Graphics/PipelineManager/PipelineManager.h"
//#include <unordered_map>
//
//class SkyDome final {
//private:
//	struct IncidentLight {
//		// 光の色
//		Vector4 color;
//
//		// 方向
//		Vector3 direction;
//
//		// ポジション
//		Vector3 pos;
//
//		// 光の強さ
//		float intensity;
//	};
//
//	struct AirStatus {
//		// 屈折率
//		float refractiveIndex;
//
//		// 単位体積当たりの分子数(余りにも値が大きいのでスケールフィルターと分ける)
//		float moleculesNum;
//
//		// スケールフィルター
//		float scaleFilter;
//
//		float wavelength;
//
//	};
//
//public:
//	struct RayleighScatteringStatus {
//		IncidentLight light;
//		AirStatus air;
//		Vector3 viewDirection;
//		Vector3 cameraPos;
//	};
//
//public:
//	SkyDome() = default;
//	SkyDome(const SkyDome&) = delete;
//	SkyDome(SkyDome&&) = delete;
//	~SkyDome() = default;
//
//	SkyDome& operator=(const SkyDome&) = delete;
//	SkyDome& operator=(SkyDome&&) = delete;
//
//public:
//	void Initialize();
//
//	void Finalize();
//
//	void Upadate();
//
//	void Draw(const class Camera& camera);
//
//public:
//	void SetTexture(Texture* const tex);
//
//private:
//	Pipeline* CreatePipeline();
//
//	Shader LoadShader();
//
//public:
//	Vector3 pos;
//	Vector3 rotate;
//	Vector3 scale;
//
//private:
//	Pipeline* pipeline_ = nullptr;
//
//	Mesh* mesh_ = nullptr;
//
//	Texture* tex_;
//
//	std::unordered_map<std::string, Mesh::CopyData> data_;
//
//	ConstBuffer<Mesh::MatrixData> wvpData_;
//
//	ConstBuffer<RayleighScatteringStatus> rayleighScattering_;
//};