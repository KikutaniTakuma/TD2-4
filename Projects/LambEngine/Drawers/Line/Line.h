#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Math/Vector4.h"
#include "Engine/Buffer/StructuredBuffer/StructuredBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"

#include "Utils/SafePtr/SafePtr.h"

#include <memory>

/// <summary>
/// 線の描画
/// </summary>
class Line {
private:
	struct VertxData {
		Mat4x4 wvp;
		Vector4 color;
	};

private:
	static constexpr uint16_t kVertexNum = 2u;
	static constexpr uint16_t kDrawMaxNumber_ = 4096u;
	static uint32_t indexCount_;

public:
	static void Initialize();

	static void Finalize();

	static void AllDraw();

private:
	static void ResetDrawCount();

private:
	static Shader shader_;

	static Lamb::SafePtr<class Pipeline> pipline_;

	static Lamb::LambPtr<ID3D12Resource> vertexBuffer_;
	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vertexView_;

	static std::unique_ptr<StructuredBuffer<VertxData, kDrawMaxNumber_>> vertData_;

public:
	Line();
	Line(const Line& right) = default;
	Line(Line&& right) noexcept = default;
	~Line() = default;

	Line& operator=(const Line& right) = default;
	Line& operator=(Line&& right)noexcept = default;

public:
	void Debug(const std::string& guiName);

	void Draw(const Mat4x4& viewProjection);

	static void Draw(const Vector3& start, const Vector3& end, const Mat4x4& viewProjection, uint32_t color);

public:
	Vector3 start;
	Vector3 end;
	uint32_t color;
};