#pragma once

#include "batch.h"
#include "../renderUtils.h"
#include "../mesh/indexedMesh.h"
#include "../buffers/bufferLayout.h"
#include "../graphics/debug/debug.h"

namespace Graphics {

template<typename UniformLayout>
class InstanceBatch {
private:
	IndexedMesh* mesh;

	BufferLayout uniformBufferLayout;
	VertexBuffer* ubo = nullptr;
	std::vector<UniformLayout> uniformBuffer;

public:
	InstanceBatch(IndexedMesh* mesh, const BufferLayout& uniformBufferLayout) : mesh(mesh), uniformBufferLayout(uniformBufferLayout) {
		ubo = new VertexBuffer(nullptr, 0);
		mesh->vao->addBuffer(ubo, uniformBufferLayout);
	}

	void add(const UniformLayout& uniform) {
		uniformBuffer.push_back(uniform);
	}

	void submit() {
		mesh->vao->bind();

		glCall(ubo->fill((const void*) uniformBuffer.data(), uniformBuffer.size() * sizeof(UniformLayout), Renderer::STREAM_DRAW));

		glDrawElementsInstanced(Renderer::TRIANGLES, mesh->triangleCount * 3, Renderer::UINT, 0, uniformBuffer.size());

		mesh->vao->unbind();

		clear();
	}

	void clear() {
		uniformBuffer.clear();
	}

	void close() {
		clear();

		ubo->close();
		mesh = nullptr;
	}
};

};