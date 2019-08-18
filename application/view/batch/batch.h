#pragma once

#include "batchConfig.h"

#include <vector>

#include "../engine/math/vec.h"

#include "../buffers/vertexArray.h"
#include "../buffers/vertexBuffer.h"
#include "../buffers/indexBuffer.h"

#include "../renderUtils.h"

template<typename Vertex>
class Batch {
private:
	VertexArray* vao = nullptr;
	VertexBuffer* vbo = nullptr;
	IndexBuffer* ibo = nullptr;

	BatchConfig config;

	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	int indexCounter;
public:
	Vertex* vertexPointer;
	unsigned int* indexPointer;
	int currentIndex;

	Batch(BatchConfig config) : config(config) {
		vao = new VertexArray();
		vbo = new VertexBuffer(nullptr, 0);
		ibo = new IndexBuffer(nullptr, 0);

		vao->addBuffer(*vbo, config.bufferLayout);

		vertexPointer = vertexBuffer.data();
		indexPointer = indexBuffer.data();

		currentIndex = 0;
	};

	inline void pushVertex(Vertex vertex) {
		*vertexPointer++ = vertex;
		indexCounter++;
	}

	inline void pushIndex(unsigned int index) {
		*indexPointer++ = currentIndex + index;
	}

	void reserve(int vertexCount, int indexCount) {
		int oldVertexBufferSize = vertexBuffer.size();
		vertexBuffer.resize(oldVertexBufferSize + vertexCount);
		vertexPointer = vertexBuffer.data() + oldVertexBufferSize;

		int oldIndexBufferSize = indexBuffer.size();
		indexBuffer.resize(oldIndexBufferSize + indexCount);
		indexPointer = indexBuffer.data() + oldIndexBufferSize;
	}

	void endIndex() {
		currentIndex += indexCounter;
		indexCounter = 0;
	}

	void submit() {
		vao->bind();

		vbo->fill((const void *) vertexBuffer.data(), vertexBuffer.size() * sizeof(Vertex), Renderer::STREAM_DRAW);
		ibo->fill((const void *) indexBuffer.data(), indexBuffer.size(), Renderer::STREAM_DRAW);

		Renderer::drawElements(Renderer::TRIANGLES, indexBuffer.size(), Renderer::UINT, nullptr);

		vbo->unbind();
		ibo->unbind();
		vao->unbind();

		clear();
	}

	void clear() {
		vertexBuffer.clear();
		indexBuffer.clear();

		vertexPointer = vertexBuffer.data();
		indexPointer = indexBuffer.data();

		currentIndex = 0;
		indexCounter = 0;
	}
};

