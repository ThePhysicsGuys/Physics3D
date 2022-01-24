#pragma once

#include <unordered_map>
#include "../util/log.h"
#include "instanceBatch.h"
#include "../buffers/bufferLayout.h"

namespace P3D::Graphics {

class InstanceBatchManager {
private:
	BufferLayout uniformBufferLayout;
	std::unordered_map<std::size_t, InstanceBatch> batches;

public:
	InstanceBatchManager(const BufferLayout& uniformBufferLayout) : uniformBufferLayout(uniformBufferLayout) {}

	void add(std::size_t mesh, const Mat4f& modelMatrix, const Comp::Material& material) {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to add a mesh that has not been registered in the mesh registry");
			return;
		}

		auto iterator = batches.find(mesh);
		if (iterator == batches.end()) 
			iterator = batches.emplace(mesh, InstanceBatch(mesh, uniformBufferLayout)).first;

		InstanceBatch::Uniform uniform {
			modelMatrix,
			material.albedo,
			material.metalness,
			material.roughness,
			material.ao
		};

		iterator->second.add(uniform);
	}									  

	void submit(std::size_t mesh) {		 
		if (batches.count(mesh) != 0)
			batches.at(mesh).submit();
	}

	void submit() {
		for (auto& [mesh, batch] : batches)
			batch.submit();
	}

	void clear() {
		for (auto& [mesh, batch] : batches)
			batch.clear();
	}
};

};