#include "core.h"
#include "mesh.h"

namespace Application {

Mesh::Mesh(IndexedMesh* indexedMesh) {
	setIndexedMesh(indexedMesh);
}

IndexedMesh* Mesh::getIndexedMesh() const {
	return indexedMesh;
}

void Mesh::setIndexedMesh(IndexedMesh* indexedMesh) {
	this->indexedMesh = indexedMesh;
}

};