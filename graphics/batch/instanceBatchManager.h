#pragma once

#include <unordered_map>
#include "instanceBatch.h"
#include "Physics3D/datastructures/smartPointers.h"
#include "../util/log.h"
#include "../buffers/bufferLayout.h"
#include "../shader/shader.h"

namespace P3D::Graphics {

template<typename Uniform>
class InstanceBatchManager {
private:
	BufferLayout uniformBufferLayout;
	std::unordered_map<GLID, URef<InstanceBatch<Uniform>>> batches;

public:
	InstanceBatchManager(const BufferLayout& uniformBufferLayout) : uniformBufferLayout(uniformBufferLayout) {}

	template<typename... Args>
	void add(GLID mesh, Args&&... uniform) {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to add a mesh that has not been registered in the mesh registry");
			return;
		}

		auto iterator = batches.find(mesh);
		if (iterator == batches.end()) 
			iterator = batches.insert(std::make_pair(mesh, std::make_unique<InstanceBatch<Uniform>>(mesh, uniformBufferLayout))).first;

		iterator->second->add(std::forward<Args>(uniform)...);
	}

	void submit(GLID mesh) {
		auto iterator = batches.find(mesh);
		if (iterator != batches.end())
			iterator->submit();
	}

	void submit(Shader* shader) {
		shader->bind();
		for (auto& [mesh, batch] : batches)
			batch->submit();
	}

	void clear() {
		for (auto& [mesh, batch] : batches)
			batch->clear();
	}

	void close() {
		for (auto& [mesh, batch] : batches)
			batch->close();
	}
};

using DefaultInstanceBatchManager = InstanceBatchManager<DefaultUniform>;

};