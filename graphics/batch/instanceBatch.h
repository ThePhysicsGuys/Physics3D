#pragma once

#include <vector>
#include <map>
#include <set>

#include "../renderer.h"
#include "../mesh/indexedMesh.h"
#include "../buffers/bufferLayout.h"
#include "../meshRegistry.h"
#include "util/systemVariables.h"

namespace P3D::Graphics {

class InstanceBatch {
public:
	struct Uniform {
		Mat4f modelMatrix = Mat4f::IDENTITY();
		Vec4f albedo = Vec4f::full(1.0f);
		float metalness = 1.0f;
		float roughness = 1.0f;
		float ao = 1.0f;
		unsigned textures = 0;
	};

private:
	std::size_t mesh = -1;
	std::vector<Uniform> uniformBuffer;
	std::map<GLID, unsigned> textureBuffer;

public:
	InstanceBatch(std::size_t id, const BufferLayout& uniformBufferLayout) : mesh(id) {
		if (id >= MeshRegistry::meshes.size()) {
			Log::error("Creating an instance batch for an invalid mesh: %d", id);
			return;
		}

		if (MeshRegistry::get(id)->uniformBuffer == nullptr) {
			VertexBuffer* uniformBuffer = new VertexBuffer(uniformBufferLayout, nullptr, 0);
			MeshRegistry::get(id)->addUniformBuffer(uniformBuffer);
		}	
	}

	~InstanceBatch() = default;
	InstanceBatch(const InstanceBatch&) = default;
	InstanceBatch(InstanceBatch&&) noexcept = default;
	InstanceBatch& operator=(const InstanceBatch&) = default;
	InstanceBatch& operator=(InstanceBatch&&) noexcept = default;

	bool add(const Mat4f& modelMatrix, const Comp::Material& material) {
		using namespace Comp;

		unsigned textureFlag = 0;
		int textureCount = material.getTextureCount();
		if (textureCount > 0) {
			int maxTextures = SystemVariables::get("MAX_TEXTURE_IMAGE_UNITS");
			int remainingTextures = maxTextures - static_cast<int>(textureBuffer.size());

			// Check if buffer is full
			if (textureCount > remainingTextures) {
				// Get all new textures
				std::set<GLID> newTextures;
				for (std::size_t index = 0; index < Material::MAP_COUNT; index++) {
					SRef<Texture> texture = material.get(index);
					if (texture == nullptr)
						continue;

					if (textureBuffer.find(texture->getID()) == textureBuffer.end())
						newTextures.insert(texture->getID());
				}

				// Fail if there is not enough space
				if (static_cast<int>(newTextures.size()) > remainingTextures)
					return false;
			}

			// Add all IDs
			for (std::size_t index = 0; index < Material::MAP_COUNT; index++) {
				SRef<Texture> texture = material.get(index);
				if (texture == nullptr)
					continue;

				addTexture(texture->getID());
			}

			// Create texture flag
			for (std::size_t index = 0; index < Material::MAP_COUNT; index++) {
				textureFlag <<= 4;

				SRef<Texture> texture = material.get(index);
				if (texture != nullptr)
					textureFlag |= textureBuffer[texture->getID()] + 1;
			}
		}

		Uniform uniform {
			modelMatrix,
			material.albedo,
			material.metalness,
			material.roughness,
			material.ao,
			textureFlag
		};

		uniformBuffer.push_back(uniform);
		
		return true;
	}

	void addTexture(GLID id) {
		auto iterator = textureBuffer.find(id);
		if (iterator == textureBuffer.end())
			textureBuffer[id] = static_cast<unsigned>(textureBuffer.size());
	}

	void bindTextures() {
		for (auto [id, unit] : textureBuffer) {
			Renderer::activeTexture(unit);
			Renderer::bindTexture2D(id);
		}

		Renderer::activeTexture(0);
	}

	void submit() {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to sumbit a mesh that has not been registered in the mesh registry");
			return;
		}

		if (uniformBuffer.empty())
			return;

		// Bind textures
		if (!textureBuffer.empty())
			bindTextures();

		// Draw meshes
		MeshRegistry::get(mesh)->fillUniformBuffer(uniformBuffer.data(), uniformBuffer.size() * sizeof(Uniform), Renderer::STREAM_DRAW);
		MeshRegistry::get(mesh)->renderInstanced(uniformBuffer.size());

		clear();
	}

	void clear() {
		uniformBuffer.clear();
	}
};

};