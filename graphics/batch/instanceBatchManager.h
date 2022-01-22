#pragma once

#include <unordered_map>
#include "../util/log.h"
#include "instanceBatch.h"
#include "../buffers/bufferLayout.h"

namespace P3D::Graphics {

template<typename Uniform>
class InstanceBatchManager {
private:
	BufferLayout uniformBufferLayout;
	std::unordered_map<GLID, InstanceBatch<Uniform>*> batches;

public:
	InstanceBatchManager(const BufferLayout& uniformBufferLayout) : uniformBufferLayout(uniformBufferLayout) {}

	template<typename... Args>
	void add(std::size_t mesh, Args&&... uniform) {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to add a mesh that has not been registered in the mesh registry");
			return;
		}

		auto iterator = batches.find(mesh);
		if (iterator == batches.end()) 
			iterator = batches.insert(std::make_pair(mesh, new InstanceBatch<Uniform>(mesh, uniformBufferLayout))).first;

		iterator->second->add(std::forward<Args>(uniform)...);
	}

	void submit(std::size_t mesh) {
		if (batches.count(mesh) != 0)
			batches.at(mesh)->submit();
	}

	void submit() {
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

};