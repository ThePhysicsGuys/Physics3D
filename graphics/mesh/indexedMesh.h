#pragma once

#include "abstractMesh.h"

#include "../buffers/bufferLayout.h"
#include "../renderer.h"

#include "../buffers/indexBuffer.h"
#include "../buffers/vertexBuffer.h"

namespace P3D::Graphics {

class IndexBuffer;
class VertexBuffer;
struct VisualShape;

class IndexedMesh : public AbstractMesh {
public:
	URef<IndexBuffer> indexBuffer = nullptr;
	URef<VertexBuffer> vertexBuffer = nullptr;
	URef<VertexBuffer> normalBuffer = nullptr;
	URef<VertexBuffer> uvBuffer = nullptr;
	URef<VertexBuffer> tangentBuffer = nullptr;
	URef<VertexBuffer> bitangentBuffer = nullptr;
	URef<VertexBuffer> uniformBuffer = nullptr;

	std::size_t vertexCount;
	std::size_t triangleCount;

	IndexedMesh(const VisualShape& shape);
	IndexedMesh(const float* vertices, const float* normals, const float* uvs, const unsigned int* indices, std::size_t vertexCount, std::size_t triangleCount);

	virtual ~IndexedMesh();
	IndexedMesh(IndexedMesh&& other);
	IndexedMesh(const IndexedMesh&) = delete;
	IndexedMesh& operator=(IndexedMesh&& other);
	IndexedMesh& operator=(const IndexedMesh&) = delete;

	void addUniformBuffer(VertexBuffer* uniformBuffer);
	void updateUniformBuffer(const void* data, size_t sizeInBytes, int offset);
	void fillUniformBuffer(const void* data, size_t sizeInBytes, GLFLAG mode);

	void render() override;
	void render(GLFLAG mode);
	void renderInstanced(size_t primitives);
	void renderInstanced(size_t primitives, GLFLAG mode);

	void close() override;
};

};