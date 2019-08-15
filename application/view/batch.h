#pragma once

#include "batchConfig.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <vector>

#include "../engine/math/vec.h"

#include "buffers\vertexArray.h"
#include "buffers\vertexBuffer.h"
#include "buffers\indexBuffer.h"

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
		vertexPointer++[0] = vertex;
		indexCounter++;
	}

	inline void pushIndex(unsigned int index) {
		indexPointer++[0] = currentIndex + index;
	}

	template<class... Vertex>
	inline void pushVertices(Vertex... vertices) {
		for (Vertex vertex : vertices) {
			pushVertex(vertex);
		}
	}

	template<class... Index>
	inline void pushIndices(Index... indices) {
		for (unsigned int index : indices) {
			pushIndex(index);
		}
	}

	void reserve(int vertexCount, int indexCount) {
		int oldVertexBufferSize = vertexBuffer.size();
		vertexBuffer.resize(oldVertexBufferSize + vertexCount);
		vertexPointer = vertexBuffer.data() + oldVertexBufferSize;

		int oldIndexBufferSize = indexBuffer.size();
		indexBuffer.resize(oldIndexBufferSize + indexCount);
		indexPointer = indexBuffer.data() + oldIndexBufferSize;
	}

	void submit() {
		vao->bind();

		vbo->bind();
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), (const void*) vertexBuffer.data(), GL_STREAM_DRAW);

		ibo->bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), (const void *) indexBuffer.data(), GL_STREAM_DRAW);

		glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

		vbo->unbind();
		ibo->unbind();

		vao->unbind();

		clear();
	}

	void endIndex() {
		currentIndex += indexCounter;
		indexCounter = 0;
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
