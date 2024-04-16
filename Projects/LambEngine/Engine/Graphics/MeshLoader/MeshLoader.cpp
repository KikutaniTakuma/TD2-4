#include "MeshLoader.h"
#include "Error/Error.h"
#include "Utils/SafePtr/SafePtr.h"
#include "../TextureManager/TextureManager.h"
#include "../../Core/DirectXDevice/DirectXDevice.h"

#include <filesystem>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh MeshLoader::LoadModel(const std::string& fileName)
{
	Assimp::Importer importer;
	Lamb::SafePtr<const aiScene> scene = ReadFile(importer, fileName);
	if (not scene->HasMeshes()) {
		throw Lamb::Error::Code<MeshLoader>("this file does not have meshes -> " + fileName, __func__);
	}

	std::filesystem::path path = fileName;
	bool isGltf = (path.extension() == ".gltf");

	//std::vector<Vertex> vertices;
	std::unordered_map<Vertex, size_t> vertices;
	size_t vertexCount = 0llu;
	std::vector<uint32_t> indices;
	uint32_t indexCount = 0;


	std::string&& directorypath = path.parent_path().string();
	std::vector<uint32_t> textures;

	LoadMtl(scene.get(), directorypath, textures);
	

	// mesh解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		Lamb::SafePtr<aiMesh> mesh = scene->mMeshes[meshIndex];
		if (not mesh->HasNormals()) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have normal -> " + fileName, __func__);
		}
		if (not mesh->HasTextureCoords(0)) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have texcoord -> " + fileName, __func__);
		}

		// 要素数追加
		vertices.reserve(vertices.size() + mesh->mNumVertices);

		// face解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			if (face.mNumIndices != 3) {
				throw Lamb::Error::Code<MeshLoader>("this file does not support -> " + fileName, __func__);
			}

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				Vertex vertex;
				vertex.pos = Vector4{ -position.x, position.y, position.z, 1.0f };
				vertex.normal = Vector3{ -normal.x, normal.y, normal.z };
				vertex.uv = Vector2{ texcoord.x, texcoord.y };

				uint32_t materialIndex = mesh->mMaterialIndex - (isGltf ? 0 : 1);

				vertex.texIndex = textures.empty() ? 0 : textures[materialIndex];

				//vertices.push_back(vertex);

				// 頂点コンテナが空
				if (vertices.empty()) {
					// 追加
					vertices.insert(std::make_pair(vertex, vertexCount));
					vertexCount++;

					// インデックス追加
					indices.emplace_back(indexCount);
					indexCount++;
				}
				// それ以外
				else {
					// そもそも同じ頂点が追加されているか
					auto isExist = vertices.find(vertex);

					// 追加されてない
					if (isExist == vertices.end()) {
						vertices.insert(std::make_pair(vertex, vertexCount));
						vertexCount++;

						// インデックス追加
						indices.emplace_back(indexCount);
						indexCount++;
					}
					// 追加してた
					else {
						// インデックス追加
						indices.emplace_back(static_cast<uint32_t>(vertices[vertex]));
					}
				}
			}
		}
	}

	//Mesh result;
	//uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Vertex) * vertices.size());

	//result.vertexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(vertexSizeInBytes);

	//Vertex* vertMap = nullptr;
	//result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	//std::copy(vertices.begin(), vertices.end(), vertMap);
	//result.vertexResource->Unmap(0, nullptr);

	//result.vertexNumber = static_cast<uint32_t>(vertices.size());
	//result.vertexView.SizeInBytes = vertexSizeInBytes;
	//result.vertexView.StrideInBytes = static_cast<uint32_t>(sizeof(Vertex));
	//result.vertexView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();

	std::vector<Vertex> verticesTmp;
	verticesTmp.resize(vertices.size());
	for (const auto& i : vertices) {
		verticesTmp[i.second] = i.first;
	}


	Mesh result;
	result.node = ReadNode(scene->mRootNode);

	uint32_t indexSizeInBytes = static_cast<uint32_t>(sizeof(uint32_t) * indices.size());
	uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Vertex) * verticesTmp.size());

	result.indexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(indexSizeInBytes);

	Lamb::SafePtr<uint32_t> indexMap = nullptr;
	result.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	std::copy(indices.begin(), indices.end(), indexMap);
	result.indexResource->Unmap(0, nullptr);

	result.indexNumber = static_cast<uint32_t>(indices.size());
	result.indexView.SizeInBytes = indexSizeInBytes;
	result.indexView.Format = DXGI_FORMAT_R32_UINT;
	result.indexView.BufferLocation = result.indexResource->GetGPUVirtualAddress();


	result.vertexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(vertexSizeInBytes);
	
	Lamb::SafePtr<Vertex> vertMap = nullptr;
	result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	std::copy(verticesTmp.begin(), verticesTmp.end(), vertMap);
	result.vertexResource->Unmap(0, nullptr);

	result.vertexNumber = static_cast<uint32_t>(verticesTmp.size());
	result.vertexView.SizeInBytes = vertexSizeInBytes;
	result.vertexView.StrideInBytes = static_cast<uint32_t>(sizeof(Vertex));
	result.vertexView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();

	return result;
}

Animations MeshLoader::LoadAnimation(const std::string& fileName)
{
	Animations result;
	Assimp::Importer importer;
	Lamb::SafePtr<const aiScene> scene = ReadFile(importer, fileName);
	if (not (scene->mNumAnimations != 0)) {
		throw Lamb::Error::Code<MeshLoader>("this file does not have meshes -> " + fileName, __func__);
	}

	result.data.resize(scene->mNumAnimations);

	for (uint32_t animtionIndex = 0; animtionIndex < scene->mNumAnimations; animtionIndex++) {
		Lamb::SafePtr<aiAnimation> animationAssimp = scene->mAnimations[animtionIndex];
		Animation& animation = result.data[animtionIndex];
		animation.duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
		
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
			Lamb::SafePtr<aiNodeAnim> nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			// Translation
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
				aiVectorKey& translation = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyFrameVector3 keyFrame;

				keyFrame.time = static_cast<float>(translation.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { -translation.mValue.x, translation.mValue.y, translation.mValue.z };
				nodeAnimation.translation.keyFrames.push_back(keyFrame);
			}
			// rotation
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
				aiQuatKey& rotate = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyFrameQuaternion keyFrame;

				keyFrame.time = static_cast<float>(rotate.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { rotate.mValue.x, -rotate.mValue.y, -rotate.mValue.z,  rotate.mValue.w };
				nodeAnimation.rotate.keyFrames.push_back(keyFrame);
			}
			// scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
				aiVectorKey& scale = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyFrameVector3 keyFrame;

				keyFrame.time = static_cast<float>(scale.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { scale.mValue.x, scale.mValue.y, scale.mValue.z };
				nodeAnimation.sacle.keyFrames.push_back(keyFrame);
			}
		}
	}

	return result;
}

const aiScene* MeshLoader::ReadFile(Assimp::Importer& importer, const std::string& fileName)
{
	std::filesystem::path path = fileName;

	// ファイル見つかんない
	if (not std::filesystem::exists(path)) {
		throw Lamb::Error::Code<MeshLoader>("this file does not find -> " + fileName, __func__);
	}
	// objかgltfではない
	if (not (path.extension() == ".obj" or path.extension() == ".gltf")) {
		throw Lamb::Error::Code<MeshLoader>("this file does not support -> " + fileName, __func__);
	}

	return importer.ReadFile(fileName.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
}

Node MeshLoader::ReadNode(aiNode* node)
{
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	
	for (uint32_t y = 0; y < 4; y++) {
		for (uint32_t x = 0; x < 4; x++) {
			result.loacalMatrix[y][x] = aiLocalMatrix[y][x];
		}
	}

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

void MeshLoader::LoadMtl(const aiScene* scene, const std::string& directorypath, std::vector<uint32_t>& result)
{
	std::vector<std::string> textureFileNames;

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		Lamb::SafePtr<aiMaterial> material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::filesystem::path tmpTextureFilePath = textureFilePath.C_Str();
			textureFileNames.push_back(directorypath + "/" + tmpTextureFilePath.filename().string());
		}
	}

	TextureManager* const textureMaanger = TextureManager::GetInstance();

	for (const auto& i : textureFileNames) {
		result.push_back(textureMaanger->LoadTexture(i));
	}
}

