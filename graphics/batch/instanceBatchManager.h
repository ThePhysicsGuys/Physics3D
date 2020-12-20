#pragma once

#include <unordered_map>

#include "instanceBatch.h"
#include "../buffers/bufferLayout.h"

namespace P3D::Graphics {

template<typename UniformLayout>
class InstanceBatchManager {
private:
	BufferLayout uniformBufferLayout;
	std::unordered_map<GLID, InstanceBatch<UniformLayout>> batches;

public:
	InstanceBatchManager(const BufferLayout& uniformBufferLayout) : uniformBufferLayout(uniformBufferLayout) {}
	
	void add(GLID mesh, const UniformLayout& uniform) {
		if (batches.count(mesh) == 0) 
			batches.emplace(mesh, InstanceBatch<UniformLayout>(mesh, uniformBufferLayout));

		batches.at(mesh)->add(uniform);
	}

	void submit(GLID mesh) {
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