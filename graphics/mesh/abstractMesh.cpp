#include "core.h"

#include "abstractMesh.h"

#include "buffers/vertexArray.h"
#include "renderer.h"

namespace P3D::Graphics {

AbstractMesh::AbstractMesh(GLFLAG renderMode) : renderMode(renderMode) {
	vao = new VertexArray();
}

AbstractMesh::AbstractMesh() : AbstractMesh(Renderer::TRIANGLES) {}

};