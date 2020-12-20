#include "core.h"

#include "indexedMesh.h"

#include "buffers/indexBuffer.h"
#include "buffers/vertexBuffer.h"
#include "buffers/vertexArray.h"
#include "visualShape.h"
#include "renderer.h"

namespace P3D::Graphics {

IndexedMesh::IndexedMesh(const VisualShape& shape) : AbstractMesh(), vertexCount(shape.vertexCount), triangleCount(shape.triangleCount) {
	float* vertices = new float[shape.vertexCount * 3];
	for (int i = 0; i < vertexCount; i++) {
		Vec3f vertex = shape.getVertex(i);
		vertices[i * 3] = vertex.x;
		vertices[i * 3 + 1] = vertex.y;
		vertices[i * 3 + 2] = vertex.z;
	}
	unsigned int* triangles = new unsigned int[shape.triangleCount * 3];
	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = shape.getTriangle(i);
		triangles[i * 3] = triangle.firstIndex;
		triangles[i * 3 + 1] = triangle.secondIndex;
		triangles[i * 3 + 2] = triangle.thirdIndex;
	}

	BufferLayout vertexBufferLayout = {
	{{ "vPosition", BufferDataType::FLOAT3 }}
	};
	vertexBuffer = new VertexBuffer(vertexBufferLayout, vertices, 3 * vertexCount * sizeof(float));

	BufferLayout normalBufferLayout = {
		{{ "vNormal", BufferDataType::FLOAT3 }}
	};
	normalBuffer = new VertexBuffer(normalBufferLayout, reinterpret_cast<float const*>(shape.normals.get()), 3 * vertexCount * sizeof(float));

	BufferLayout uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};
	uvBuffer = new VertexBuffer(uvBufferLayout, reinterpret_cast<float const*>(shape.uvs.get()), 2 * vertexCount * sizeof(float));

	BufferLayout tangentBufferLayout = {
		{{ "vTangent", BufferDataType::FLOAT3 }}
	};
	tangentBuffer = new VertexBuffer(tangentBufferLayout, reinterpret_cast<float const*>(shape.tangents.get()), 3 * vertexCount * sizeof(float));

	BufferLayout bitangentBufferLayout = {
		{{ "vBitangent", BufferDataType::FLOAT3 }}
	};
	bitangentBuffer = new VertexBuffer(bitangentBufferLayout, reinterpret_cast<float const*>(shape.bitangents.get()), 3 * vertexCount * sizeof(float));

	indexBuffer = new IndexBuffer(triangles, 3 * triangleCount);

	vao->addBuffer(vertexBuffer);
	vao->addBuffer(normalBuffer);
	vao->addBuffer(uvBuffer);
	vao->addBuffer(tangentBuffer);
	vao->addBuffer(bitangentBuffer);
}

IndexedMesh::IndexedMesh(const float* vertices, const float* normals, const float* uvs, const unsigned int* indices, const int vertexCount, const int triangleCount) : AbstractMesh(), vertexCount(vertexCount), triangleCount(triangleCount) {
	BufferLayout vertexBufferLayout = {
		{{ "vposition", BufferDataType::FLOAT3 }}
	};
	vertexBuffer = new VertexBuffer(vertexBufferLayout, vertices, 3 * vertexCount * sizeof(float));

	BufferLayout normalBufferLayout = {
		{{ "vnormal", BufferDataType::FLOAT3 }}
	};
	normalBuffer = new VertexBuffer(normalBufferLayout, normals, 3 * vertexCount * sizeof(float));

	BufferLayout uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};
	uvBuffer = new VertexBuffer(uvBufferLayout, uvs, 2 * vertexCount * sizeof(float));

	indexBuffer = new IndexBuffer(indices, 3 * triangleCount);

	// TODO Generate tangents and bitangents
	BufferLayout tangentBufferLayout = {
		{{ "vTangent", BufferDataType::FLOAT3 }}
	};
	tangentBuffer = new VertexBuffer(tangentBufferLayout, nullptr, 0);

	BufferLayout bitangentBufferLayout = {
		{{ "vBitangent", BufferDataType::FLOAT3 }}
	};
	bitangentBuffer = new VertexBuffer(bitangentBufferLayout, nullptr, 0);

	vao->addBuffer(vertexBuffer);
	vao->addBuffer(normalBuffer);
	vao->addBuffer(uvBuffer);
	vao->addBuffer(tangentBuffer);
	vao->addBuffer(bitangentBuffer);
}

IndexedMesh::~IndexedMesh() {
	close();
	Log::debug("Closed indexed mesh");
}

IndexedMesh::IndexedMesh(IndexedMesh&& other) {
	vao = other.vao;

	indexBuffer = other.indexBuffer;
	vertexBuffer = other.vertexBuffer;
	normalBuffer = other.normalBuffer;
	uvBuffer = other.uvBuffer;
	tangentBuffer = other.tangentBuffer;
	bitangentBuffer = other.bitangentBuffer;
	uniformBuffer = other.uniformBuffer;

	vertexCount = other.vertexCount;
	triangleCount = other.triangleCount;

	// Reset so they cant be deleted by close()
	other.vao = nullptr;

	other.indexBuffer = nullptr;
	other.vertexBuffer = nullptr;
	other.normalBuffer = nullptr;
	other.uvBuffer = nullptr;
	other.bitangentBuffer = nullptr;
	other.tangentBuffer = nullptr;
	other.uniformBuffer = nullptr;

	other.close();
}

IndexedMesh& IndexedMesh::operator=(IndexedMesh&& other) {
	if (this != &other) {
		close();
		std::swap(vao, other.vao);

		std::swap(indexBuffer, other.indexBuffer);
		std::swap(vertexBuffer, other.vertexBuffer);
		std::swap(normalBuffer, other.normalBuffer);
		std::swap(uvBuffer, other.uvBuffer);
		std::swap(tangentBuffer, other.tangentBuffer);
		std::swap(bitangentBuffer, other.bitangentBuffer);
		std::swap(uniformBuffer, other.uniformBuffer);

		std::swap(vertexCount, other.vertexCount);
		std::swap(triangleCount, other.triangleCount);
	}

	return *this;
}

void IndexedMesh::render() {
	render(Renderer::FILL);
}

void IndexedMesh::addUniformBuffer(VertexBuffer* uniformBuffer) {
	this->uniformBuffer = uniformBuffer;

	vao->addBuffer(uniformBuffer);
}

void IndexedMesh::updateUniformBuffer(const void* data, size_t sizeInBytes, int offset) {
	if (uniformBuffer)
		uniformBuffer->update(data, sizeInBytes, offset);
	else
		Log::error("Updating non existing uniform buffer");
}

void IndexedMesh::fillUniformBuffer(const void* data, size_t sizeInBytes, GLFLAG mode) {
	if (uniformBuffer) 
		uniformBuffer->fill(data, sizeInBytes, mode);
	else
		Log::error("Filling non existing uniform buffer");
}

void IndexedMesh::render(GLFLAG mode) {
	vao->bind();
	indexBuffer->bind();

	Renderer::polygonMode(Renderer::FRONT_AND_BACK, mode);
	Renderer::drawElements(renderMode, triangleCount * 3, Renderer::UINT, 0);
	Renderer::polygonMode(Renderer::FRONT_AND_BACK, Renderer::FILL);
}

void IndexedMesh::renderInstanced(size_t primitives) {
	renderInstanced(primitives, Renderer::FILL);
}

void IndexedMesh::renderInstanced(size_t primitives, GLFLAG mode) {
	vao->bind();
	indexBuffer->bind();

	Renderer::polygonMode(Renderer::FRONT_AND_BACK, mode);
	Renderer::drawElementsInstanced(renderMode, triangleCount * 3, Renderer::UINT, 0, primitives);
	Renderer::polygonMode(Renderer::FRONT_AND_BACK, Renderer::FILL);
}

void IndexedMesh::close() {
	if (vertexBuffer)
		vertexBuffer->close();
	if (indexBuffer)
		indexBuffer->close();
	if (normalBuffer)
		normalBuffer->close();
	if (uvBuffer)
		uvBuffer->close();
	if (tangentBuffer)
		tangentBuffer->close();
	if (bitangentBuffer)
		bitangentBuffer->close();
	if (uniformBuffer)
		uniformBuffer->close();
	if (vao)
		vao->close();

	vao = nullptr;

	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	normalBuffer = nullptr;
	uvBuffer = nullptr;
	tangentBuffer = nullptr;
	bitangentBuffer = nullptr;
	uniformBuffer = nullptr;

	triangleCount = 0;
	vertexCount = 0;
}

};