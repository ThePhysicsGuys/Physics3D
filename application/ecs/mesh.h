#pragma once

#include "../engine/ecs/component.h"

namespace Application {

class IndexedMesh;

/*
	A component representing a renderable mesh
*/
class Mesh : public Engine::Component {
private:
	/*
		The mesh of this Mesh component
	*/
	IndexedMesh* indexedMesh;

public:
	DEFINE_COMPONENT(Mesh, true);

	/*
		Creates an new Mesh component with the given indexed mesh
	*/
	Mesh(IndexedMesh* indexedMesh);

	/*
		Returns the indexed mesh of this component
	*/
	IndexedMesh* getIndexedMesh() const;

	/*
		Sets the indexed mesh of this component to the given value
	*/
	void setIndexedMesh(IndexedMesh* indexedMesh);
};

}