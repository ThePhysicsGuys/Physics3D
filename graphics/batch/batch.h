#pragma once

#include "core.h"

#include "batchConfig.h"

#include "../buffers/vertexArray.h"
#include "../buffers/vertexBuffer.h"
#include "../buffers/indexBuffer.h"

#include "../renderer.h"

namespace Graphics {

template<typename Vertex>
class Batch {
protected:
	VertexArray* vao = nullptr;
	VertexBuffer* vbo = nullptr;
	IndexBuffer* ibo = nullptr;

	BatchConfig config;

	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	size_t indexCounter;

public:
	Vertex* vertexPointer;
	unsigned int* indexPointer;
	size_t currentIndex;

	Batch(const BatchConfig& config) : config(config) {
		vao = new VertexArray();
		vbo = new VertexBuffer(nullptr, 0);
		ibo = new IndexBuffer(nullptr, 0);

		vao->addBuffer(vbo, config.bufferLayout);

		vertexPointer = vertexBuffer.data();
		indexPointer = indexBuffer.data();

		indexCounter = 0;
		currentIndex = 0;
	};

	Batch(const Batch&) = delete;

	Batch& operator=(const Batch&) = delete;

	~Batch() {
		close();
	}

	Batch(Batch&& other) {
		vao = other.vao;
		other.vao = nullptr;

		vbo = other.vbo;
		other.vbo = nullptr;

		ibo = other.ibo;
		other.ibo = nullptr;

		indexCounter = other.indexCounter;
		other.indexCounter = 0;

		currentIndex = other.currentIndex;
		other.currentIndex = 0;

		vertexBuffer = std::move(other.vertexBuffer);
		other.vertexBuffer.clear();

		indexBuffer = std::move(other.indexBuffer);
		other.indexBuffer.clear();

		vertexPointer = other.vertexPointer;
		other.vertexPointer = other.vertexBuffer.data();

		indexPointer = other.indexPointer;
		other.indexPointer = other.indexBuffer.data();

		config = std::move(other.config);
	}

	Batch& operator=(Batch&& other) {
		if (this != &other) {
			close();
			std::swap(vao, other.vao);
			std::swap(vbo, other.vbo);
			std::swap(ibo, other.ibo);

			config = std::move(other.config);
			vertexBuffer = std::move(other.vertexBuffer);
			indexBuffer = std::move(other.indexBuffer);

			indexCounter = std::move(other.indexBuffer);
			currentIndex = std::move(other.currentIndex);

			vertexPointer = std::move(other.vertexPointer);
			indexPointer = std::move(other.indexPointer);
		}

		return *this;
	}

	inline void pushVertex(Vertex vertex) {
		*vertexPointer++ = vertex;
		indexCounter++;
	}

	inline void pushVertices(std::initializer_list<Vertex> vertices) {
		for (Vertex vertex : vertices) {
			*vertexPointer++ = vertex;
			indexCounter++;
		}
	}

	inline void pushIndex(size_t index) {
		*indexPointer++ = currentIndex + index;
	}

	inline void pushIndices(std::initializer_list<size_t> indices) {
		for (size_t index : indices)
			*indexPointer++ = currentIndex + index;
	}

	void reserve(size_t vertexCount, size_t indexCount) {
		size_t oldVertexBufferSize = vertexBuffer.size();
		vertexBuffer.resize(oldVertexBufferSize + vertexCount);
		vertexPointer = vertexBuffer.data() + oldVertexBufferSize;

		size_t oldIndexBufferSize = indexBuffer.size();
		indexBuffer.resize(oldIndexBufferSize + indexCount);
		indexPointer = indexBuffer.data() + oldIndexBufferSize;
	}

	void endIndex() {
		currentIndex += indexCounter;
		indexCounter = 0;
	}

	virtual void submit() {
		vao->bind();

		vbo->fill((const void*) vertexBuffer.data(), vertexBuffer.size() * sizeof(Vertex), Graphics::Renderer::STREAM_DRAW);
		ibo->fill((const unsigned int*) indexBuffer.data(), indexBuffer.size(), Graphics::Renderer::STREAM_DRAW);

		Graphics::Renderer::drawElements(config.type, indexBuffer.size(), Graphics::Renderer::UINT, nullptr);

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

	void close() {
		clear();

		vao->close();
		vbo->close();
		ibo->close();
	}
};

};