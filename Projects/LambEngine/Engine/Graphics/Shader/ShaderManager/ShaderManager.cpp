#include "ShaderManager.h"
#include "../ShaderFactory/ShaderFactory.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include <cassert>
#include <format>
#include "Utils/ExecutionLog/ExecutionLog.h"

Lamb::SafePtr<ShaderManager> ShaderManager::instance_ = nullptr;

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
	
}

void ShaderManager::Initialize() {
	instance_.reset(new ShaderManager());
}

void ShaderManager::Finalize() {
	instance_.reset();
}

IDxcBlob* const ShaderManager::LoadVertexShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = vertexShader_.find(fileName);
	if (itr == vertexShader_.end()) {
		Lamb::SafePtr shader = shaderFactory_->CompileShader(ConvertString(fileName), L"vs_6_0");
		shader.NullPointerException<ShaderManager>(__func__);
		vertexShader_.insert(std::make_pair(fileName, shader.get()));
	}
	return vertexShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadHullShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = hullShader_.find(fileName);
	if (itr == hullShader_.end()) {
		Lamb::SafePtr shader = shaderFactory_->CompileShader(ConvertString(fileName), L"hs_6_0");
		shader.NullPointerException<ShaderManager>(__func__);
		hullShader_.insert(std::make_pair(fileName, shader.get()));
	}

	return hullShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadDomainShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = domainShader_.find(fileName);
	if (itr == domainShader_.end()) {
		Lamb::SafePtr shader = shaderFactory_->CompileShader(ConvertString(fileName), L"ds_6_0");
		shader.NullPointerException<ShaderManager>(__func__);
		domainShader_.insert(std::make_pair(fileName, shader.get()));
	}
	return domainShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = geometoryShader_.find(fileName);
	if (itr == geometoryShader_.end()) {
		Lamb::SafePtr shader = shaderFactory_->CompileShader(ConvertString(fileName), L"gs_6_0");
		shader.NullPointerException<ShaderManager>(__func__);
		geometoryShader_.insert(std::make_pair(fileName, shader.get()));
	}
	return geometoryShader_[fileName].Get();
}
IDxcBlob* const ShaderManager::LoadPixelShader(const std::string& fileName) {
	if (fileName.empty()) {
		return nullptr;
	}

	auto itr = pixelShader_.find(fileName);
	if (itr == pixelShader_.end()) {
		Lamb::SafePtr shader = shaderFactory_->CompileShader(ConvertString(fileName), L"ps_6_0");
		shader.NullPointerException<ShaderManager>(__func__);
		pixelShader_.insert(std::make_pair(fileName, shader.get()));
	}
	return pixelShader_[fileName].Get();
}