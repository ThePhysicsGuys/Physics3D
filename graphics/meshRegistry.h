#pragma once

#include "visualData.h"
#include "visualShape.h"
#include "mesh/indexedMesh.h"
#include "../physics/geometry/shapeClass.h"

namespace P3D::Graphics::MeshRegistry {

extern std::vector<IndexedMesh*> meshes;
	
extern std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
extern VisualData box;
extern VisualData sphere;
extern VisualData cylinder;

void init();

VisualData addMeshShape(const VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass, const VisualShape& mesh);
VisualData registerMeshFor(const ShapeClass* shapeClass);
VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);

VisualShape createCube(float size);
VisualShape createBox(float width, float height, float depth);
VisualShape createSphere(double radius, int steps = 1);
VisualShape createCylinder(int sides, double radius, double height);

};
