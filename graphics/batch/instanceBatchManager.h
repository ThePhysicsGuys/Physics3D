#pragma once

#include <unordered_map>

#include "instanceBatch.h"
#include "../util/log.h"
#include "../shader/shader.h"
#include "../buffers/bufferLayout.h"

namespace P3D::Graphics {

class InstanceBatchManager {
private:
	SRef<Shader> shader;
	BufferLayout uniformBufferLayout;
	std::unordered_map<std::size_t, std::vector<InstanceBatch>> batches;

public:
	InstanceBatchManager(SRef<Shader> shader, const BufferLayout& uniformBufferLayout) : shader(shader), uniformBufferLayout(uniformBufferLayout) {}

	void add(std::size_t mesh, const Mat4f& modelMatrix, const Comp::Material& material) {
		if (mesh >= MeshRegistry::meshes.size()) {
			Log::error("Trying to add a mesh that has not been registered in the mesh registry");
			return;
		}

		auto batchIterator = batches.find(mesh);
		if (batchIterator == batches.end())
			batchIterator = batches.emplace(mesh, std::vector { InstanceBatch(mesh, uniformBufferLayout) }).first;

		std::vector<InstanceBatch>& currentBatches = batchIterator->second;
		InstanceBatch& currentBatch = currentBatches[currentBatches.size() - 1];
		bool success = currentBatch.add(modelMatrix, material);

		if (!success) {
			InstanceBatch newBatch(mesh, uniformBufferLayout);
			newBatch.add(modelMatrix, material);

			currentBatches.push_back(newBatch);
		}
	}									  

	void submit(std::size_t mesh) {
		shader->bind();

		auto iterator = batches.find(mesh);
		if (iterator != batches.end()) 
			for (InstanceBatch& batch : iterator->second)
				batch.submit();
	}

	void submit() {
		shader->bind();

		for (auto& [mesh, batches] : this->batches)
			for (auto& batch : batches)
				batch.submit();
	}

	void clear() {
		for (auto& [mesh, batches] : this->batches)
			for (auto& batch : batches)
				batch.clear();
	}
};

};