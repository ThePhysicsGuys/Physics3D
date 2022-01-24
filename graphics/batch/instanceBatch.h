#pragma once

#include <vector>
#include <map>
#include "../renderer.h"
#include "../mesh/indexedMesh.h"
#include "../buffers/bufferLayout.h"
#include "../meshRegistry.h"

namespace P3D::Graphics {

class InstanceBatch {
public:
	struct Uniform {
		Mat4f modelMatrix = Mat4f::IDENTITY();
		Vec4f albedo = Vec4f::full(1.0f);
		float metalness = 1.0f;
		float roughness = 1.0f;
		float ao = 1.0f;
	};

private:
	std::size_t mesh = -1;
	std::vector<Uniform> uniformBuffer;
	std::map<GLID, std::size_t> textureBuffer;

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

	template<typename... Args>
	void add(const Uniform& uniform) {
		uniformBuffer.push_back(uniform);
	}

	void addTexture(GLID id) {
		auto iterator = textureBuffer.find(id);
		if (iterator == textureBuffer.end())
			textureBuffer[id] = textureBuffer.size();
	}

	void submit() {
		if (mesh < 0 || mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to sumbit a mesh that has not been registered in the mesh registry");
			return;
		}

		if (uniformBuffer.empty())
			return;
		
		MeshRegistry::get(mesh)->fillUniformBuffer(static_cast<const void*>(uniformBuffer.data()), uniformBuffer.size() * sizeof(Uniform), Renderer::STREAM_DRAW);
		MeshRegistry::get(mesh)->renderInstanced(uniformBuffer.size());

		clear();
	}

	void clear() {
		uniformBuffer.clear();
	}
};

};