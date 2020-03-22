#include "core.h"

#include "abstractMesh.h"

#include "buffers/vertexArray.h"
#include "renderer.h"

namespace Graphics {

AbstractMesh::AbstractMesh(GLFLAG renderMode) : renderMode(renderMode) {
	vao = new VertexArray();
}

AbstractMesh::AbstractMesh() : AbstractMesh(Renderer::TRIANGLES) {}

};