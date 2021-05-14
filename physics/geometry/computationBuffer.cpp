#include "computationBuffer.h"

#include "../../util/log.h"
#include "genericIntersection.h"

namespace P3D {
ComputationBuffers::ComputationBuffers(int initialVertCount, int initialTriangleCount) :
	vertexCapacity(initialVertCount), triangleCapacity(initialTriangleCount) {
	createVertexBuffersUnsafe(initialVertCount);
	createTriangleBuffersUnsafe(initialTriangleCount);
}

void ComputationBuffers::ensureCapacity(int vertCapacity, int triangleCapacity) {
	if(this->vertexCapacity < vertCapacity) {
		Log::debug("Increasing vertex buffer capacity from %d to %d", this->vertexCapacity, vertCapacity);
		deleteVertexBuffers();
		createVertexBuffersUnsafe(vertCapacity);
	}
	if(this->triangleCapacity < triangleCapacity) {
		Log::debug("Increasing vertex buffer capacity from %d to %d", this->triangleCapacity, triangleCapacity);
		deleteTriangleBuffers();
		createTriangleBuffersUnsafe(triangleCapacity);
	}
}

ComputationBuffers::~ComputationBuffers() {
	deleteVertexBuffers();
	deleteTriangleBuffers();
}

void ComputationBuffers::createVertexBuffersUnsafe(int newVertexCapacity) {
	vertBuf = new Vec3f[newVertexCapacity];
	knownVecs = new MinkowskiPointIndices[newVertexCapacity];
	this->vertexCapacity = newVertexCapacity;
}

void ComputationBuffers::createTriangleBuffersUnsafe(int newTriangleCapacity) {
	triangleBuf = new Triangle[newTriangleCapacity];
	neighborBuf = new TriangleNeighbors[newTriangleCapacity];
	edgeBuf = new EdgePiece[newTriangleCapacity];
	removalBuf = new int[newTriangleCapacity];
	this->triangleCapacity = newTriangleCapacity;
}

void ComputationBuffers::deleteVertexBuffers() {
	delete[] vertBuf;
	delete[] knownVecs;
}

void ComputationBuffers::deleteTriangleBuffers() {
	delete[] triangleBuf;
	delete[] neighborBuf;
	delete[] edgeBuf;
	delete[] removalBuf;
}
};