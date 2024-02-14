#include "ShaderManager.h"
#include "../ShaderFactory/ShaderFactory.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include <cassert>
#include <format>
#include "Utils/ExecutionLog/ExecutionLog.h"

ShaderManager* ShaderManager::instance_ = nullptr;

ShaderManager::ShaderManager() {
	vertexShader_.clear();
	hullShader_.clear();
	domainShader_.clear();
	geometoryShader_.clear();
	pixelShader_.clear();

	ShaderFactory::Initialize();

	shaderFactory_ = ShaderFactory::GetInstance();

	Lamb::AddLog("Initialize ShaderManager succeeded");
}

ShaderManager::~ShaderManager() {
	ShaderFactory::Finalize();
}

void ShaderManager::Initialize() {
	instance_ = new ShaderManager();
	assert(instance_);
}

void ShaderManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

IDxcBlob* const ShaderManager::LoadVertexShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = vertexShader_.find(fileName);
	if (itr == vertexShader_.end()) {
		IDxcBlob* shader = shaderFactory_->CompileShader(ConvertString(fileName), L"vs_6_0");
		assert(shader);
		vertexShader_.insert(std::make_pair(fileName, shader));
	}
	return vertexShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadHullShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = hullShader_.find(fileName);
	if (itr == hullShader_.end()) {
		IDxcBlob* shader = shaderFactory_->CompileShader(ConvertString(fileName), L"hs_6_0");
		assert(shader);
		hullShader_.insert(std::make_pair(fileName, shader));
	}

	return hullShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadDomainShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = domainShader_.find(fileName);
	if (itr == domainShader_.end()) {
		IDxcBlob* shader = shaderFactory_->CompileShader(ConvertString(fileName), L"ds_6_0");
		assert(shader);
		domainShader_.insert(std::make_pair(fileName, shader));
	}
	return domainShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = geometoryShader_.find(fileName);
	if (itr == geometoryShader_.end()) {
		IDxcBlob* shader = shaderFactory_->CompileShader(ConvertString(fileName), L"gs_6_0");
		assert(shader);
		geometoryShader_.insert(std::make_pair(fileName, shader));
	}
	return geometoryShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadPixelShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = pixelShader_.find(fileName);
	if (itr == pixelShader_.end()) {
		IDxcBlob* shader = shaderFactory_->CompileShader(ConvertString(fileName), L"ps_6_0");
		assert(shader);
		pixelShader_.insert(std::make_pair(fileName, shader));
	}
	return pixelShader_[fileName].Get();
}