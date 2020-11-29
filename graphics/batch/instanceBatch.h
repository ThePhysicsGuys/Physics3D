#pragma once

#include <vector>
#include "batch.h"
#include "../renderer.h"
#include "../mesh/indexedMesh.h"
#include "../buffers/bufferLayout.h"
#include "../meshRegistry.h"

namespace P3D::Graphics {

template<typename UniformLayout>
class InstanceBatch {
private:
	GLID mesh;
	VertexBuffer* ubo = nullptr;
	std::vector<UniformLayout> uniformBuffer;

public:
	InstanceBatch(GLID mesh, const BufferLayout& uniformBufferLayout) : mesh(mesh) {
		ubo = new VertexBuffer(uniformBufferLayout, nullptr, 0);
		Graphics::MeshRegistry::meshes[mesh]->vao->addBuffer(ubo);
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