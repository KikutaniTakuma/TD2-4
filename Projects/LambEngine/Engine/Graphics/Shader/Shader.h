#pragma once

/// <summary>
/// シェーダーをまとめる構造体
/// </summary>
struct Shader {
	struct IDxcBlob* vertex = nullptr;
	struct IDxcBlob* hull = nullptr;
	struct IDxcBlob* domain = nullptr;
	struct IDxcBlob* geometory = nullptr;
	struct IDxcBlob* pixel = nullptr;
};