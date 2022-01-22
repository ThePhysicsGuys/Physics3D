#pragma once

#include "extendedTriangleMesh.h"
#include "mesh/indexedMesh.h"
#include <Physics3D/geometry/shapeClass.h>

namespace P3D::Graphics::MeshRegistry {

extern std::vector<IndexedMesh*> meshes;
	
extern std::map<const ShapeClass*, Comp::Mesh> shapeClassMeshIds;

extern Comp::Mesh box;
extern Comp::Mesh sphere;
extern Comp::Mesh cylinder;
extern Comp::Mesh wedge;
extern Comp::Mesh corner;
extern Comp::Mesh hexagon;

void init();

Comp::Mesh registerShape(const ExtendedTriangleMesh& mesh);
Comp::Mesh registerShapeClass(const ShapeClass* shapeClass, const ExtendedTriangleMesh& mesh);
Comp::Mesh registerShapeClass(const ShapeClass* shapeClass);
Comp::Mesh getMesh(const ShapeClass* shapeClass);
std::size_t getID(const ShapeClass* shapeClass);
IndexedMesh* get(std::size_t id);
IndexedMesh* get(const Comp::Mesh& mesh);

ExtendedTriangleMesh createCube(float size);
ExtendedTriangleMesh createBox(float width, float height, float depth);
ExtendedTriangleMesh createSphere(double radius, int steps = 1);
ExtendedTriangleMesh createCylinder(int sides, double radius, double height);
ExtendedTriangleMesh createHexagon(float radius, float height);

};
