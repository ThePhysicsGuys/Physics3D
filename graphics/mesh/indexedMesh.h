#pragma once

#include "abstractMesh.h"

#include "../buffers/bufferLayout.h"
#include "../renderer.h"

namespace Graphics {

class IndexBuffer;
class VertexBuffer;
struct VisualShape;

class IndexedMesh : public AbstractMesh {
public:
	BufferLayout vertexBufferLayout;
	BufferLayout normalBufferLayout;
	BufferLayout uvBufferLayout;

	IndexBuffer* indexBuffer = nullptr;
	VertexBuffer* vertexBuffer = nullptr;
	VertexBuffer* normalBuffer = nullptr;
	VertexBuffer* uvBuffer = nullptr;

	BufferLayout uniformBufferLayout;
	VertexBuffer* uniformBuffer = nullptr;

	int vertexCount;
	int triangleCount;

	IndexedMesh(const VisualShape& shape);
	IndexedMesh(const float* vertices, const float* normals, const float* uvs, const unsigned int* indices, const int vertexCount, const int triangleCount);

	~IndexedMesh();
	IndexedMesh(IndexedMesh&& other);
	IndexedMesh(const IndexedMesh&) = delete;
	IndexedMesh& operator=(IndexedMesh&& other);
	IndexedMesh& operator=(const IndexedMesh&) = delete;

	void addUniformBuffer(VertexBuffer* uniformBuffer, const BufferLayout& uniformBufferLayout);
	void updateUniformBuffer(const void* data, size_t sizeInBytes, int offset);

	void render() override;
	void render(GLFLAG mode);
	void renderInstanced(size_t primitives);
	void renderInstanced(GLFLAG mode, size_t primitives);

	void close() override;
};

};