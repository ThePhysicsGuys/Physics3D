#include "core.h"

#include "abstractMesh.h"

#include "buffers/vertexArray.h"
#include "renderer.h"

namespace Graphics {

AbstractMesh::AbstractMesh(unsigned int rendermode) : renderMode(renderMode) {
	vao = new VertexArray();
};

AbstractMesh::AbstractMesh() : renderMode(Graphics::Renderer::TRIANGLES) {
	vao = new VertexArray();
};

};