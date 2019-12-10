#pragma once

#include "core.h"

#include "instanceBatch.h"
#include "../buffers/bufferLayout.h"
#include "../mesh/indexedMesh.h"

struct UniformLayout {
	Mat4f transform;
	Vec4f ambient;
	Vec3f diffuse;
	Vec3f specular;
	float reflectance;

	UniformLayout(Mat4f transform, Vec4f ambient, Vec3f diffuse, Vec3f specular, float reflectance) : transform(transform), ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {};
};

class InstanceBatchManager {
private:
	std::unordered_map<IndexedMesh*, InstanceBatch<UniformLayout>*> batches;
	
	BufferLayout uniformBufferLayout = BufferLayout({
		BufferElement("transform", BufferDataType::MAT4, true),
		BufferElement("ambient", BufferDataType::FLOAT4, true),
		BufferElement("diffuse", BufferDataType::FLOAT3, true),
		BufferElement("specular", BufferDataType::FLOAT3, true),
		BufferElement("reflectance", BufferDataType::FLOAT, true)
	});

public:
	void add(IndexedMesh* mesh, const UniformLayout& uniform) {
		if (batches.count(mesh) == 0) {
			InstanceBatch<UniformLayout>* batch = new InstanceBatch<UniformLayout>(mesh, uniformBufferLayout);
			batches[mesh] = batch;
		}
			
		batches.at(mesh)->add(uniform);
	}

	void submit(IndexedMesh* mesh) {
		if (batches.count(mesh) != 0)
			batches.at(mesh)->submit();
	};

	void submit() {
		for (auto batchIterator : batches)
			batchIterator.second->submit();
	};

	void clear() {
		for (auto batchIterator : batches)
			batchIterator.second->clear();
	}

	void close() {
		for (auto batchIterator : batches)
			batchIterator.second->close();
	}
};