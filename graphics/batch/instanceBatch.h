#pragma once

#include <vector>
#include "batch.h"
#include "../renderer.h"
#include "../mesh/indexedMesh.h"
#include "../buffers/bufferLayout.h"
#include "../meshRegistry.h"

namespace P3D::Graphics {

template<typename Uniform>
class InstanceBatch {
private:
	GLID mesh = -1;
	std::vector<Uniform> uniformBuffer;

public:
	InstanceBatch(GLID mesh, const BufferLayout& uniformBufferLayout) : mesh(mesh) {
		if (mesh < 0 || MeshRegistry::meshes.size() <= mesh) {
			Log::error("Creating an instance batch for an invalid mesh: %d", mesh);
#ifdef _MSC_VER
			__debugbreak();
#else
			throw "Creating an instance batch for an invalid mesh";
#endif
			return;
		}

		if (MeshRegistry::meshes[mesh]->uniformBuffer == nullptr) {
			VertexBuffer* uniformBuffer = new VertexBuffer(uniformBufferLayout, nullptr, 0);
			MeshRegistry::meshes[mesh]->addUniformBuffer(uniformBuffer);
		}	
	}

	~InstanceBatch() {
		Log::error("Deleted instance batch");
		close();
	}

	template<typename... Args>
	void add(Args&&... uniform) {
		uniformBuffer.emplace_back(std::forward<Args>(uniform)...);
	}

	void submit() {
		if (mesh < 0 || mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to sumbit a mesh that has not been registered in the mesh registry");
			return;
		}

		if (uniformBuffer.empty())
			return;
		
		MeshRegistry::meshes[mesh]->fillUniformBuffer((const void*) uniformBuffer.data(), uniformBuffer.size() * sizeof(Uniform), Renderer::STREAM_DRAW);
		MeshRegistry::meshes[mesh]->renderInstanced(uniformBuffer.size());

		clear();
	}

	void clear() {
		uniformBuffer.clear();
	}

	void close() {
		clear();
		
		mesh = -1;
	}
};

};