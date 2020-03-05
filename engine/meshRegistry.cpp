#include "core.h"
#include "meshRegistry.h"

#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/basicShapes.h"

#include "../graphics/visualShape.h"
#include "../engine/visualData.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../physics/geometry/shapeClass.h"

#define PI 3.14159265359

namespace Engine::MeshRegistry {

std::vector<Graphics::IndexedMesh*> meshes;
std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
VisualData box;
VisualData sphere;
VisualData cylinder;

// Generates a cylinder with 
static Graphics::VisualShape createCylinder(int sides, double radius, double height) {
	assert(sides >= 2);
	int vertexCount = sides * 4;
	Vec3f* vecBuf = new Vec3f[vertexCount];


	// vertices
	for(int i = 0; i < sides; i++) {
		double angle = i * PI * 2 / sides;
		Vec3f bottom(cos(angle) * radius, sin(angle) * radius, height / 2);
		Vec3f top(cos(angle) * radius, sin(angle) * radius, -height / 2);
		vecBuf[i * 2] = bottom;
		vecBuf[i * 2 + 1] = top;
		vecBuf[i * 2 + sides * 2] = bottom;
		vecBuf[i * 2 + 1 + sides * 2] = top;
	}

	int triangleCount = sides * 2 + (sides - 2) * 2;
	Triangle* triangleBuf = new Triangle[triangleCount];

	// sides
	for(int i = 0; i < sides; i++) {
		int botLeft = i * 2;
		int botRight = ((i + 1) % sides) * 2;
		triangleBuf[i * 2] = Triangle{botLeft, botLeft + 1, botRight}; // botLeft, botRight, topLeft
		triangleBuf[i * 2 + 1] = Triangle{botRight + 1, botRight, botLeft + 1}; // topRight, topLeft, botRight
	}

	Vec3f* normals = new Vec3f[vertexCount];

	for(int i = 0; i < sides * 2; i++) {
		Vec3f vertex = vecBuf[i];
		normals[i] = normalize(Vec3(vertex.x, vertex.y, 0));
	}

	Triangle* capOffset = triangleBuf + sides * 2Ui64;
	// top and bottom
	for(int i = 0; i < sides - 2; i++) { // common corner is i=0
		capOffset[i] = Triangle{sides * 2 + 0, sides * 2 + (i + 1) * 2, sides * 2 + (i + 2) * 2};
		capOffset[i + (sides - 2)] = Triangle{sides * 2 + 1, sides * 2 + (i + 2) * 2 + 1, sides * 2 + (i + 1) * 2 + 1};
	}

	for(int i = 0; i < sides; i++) {
		normals[i * 2 + sides * 2] = Vec3f(0, 0, 1);
		normals[i * 2 + sides * 2 + 1] = Vec3f(0, 0, -1);
	}

	return Graphics::VisualShape(vecBuf, SharedArrayPtr<const Vec3f>(normals), triangleBuf, vertexCount, triangleCount);
}

Graphics::VisualShape createSphere(double radius, int steps) {
	Polyhedron sphere(Library::createSphere(radius, steps));
	Graphics::VisualShape sphereShape = Graphics::VisualShape(sphere);
	Vec3f* normalBuf = new Vec3f[sphereShape.vertexCount];
	sphereShape.computeNormals(normalBuf);
	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuf);
	return sphereShape;
}

void init() {
	sphere = registerMeshFor(sphereClass);
	box = registerMeshFor(boxClass);
	cylinder = registerMeshFor(cylinderClass, createCylinder(64, 1.0, 2.0));
}


VisualData addMeshShape(const Graphics::VisualShape& s) {
	int size = (int) meshes.size();
	//Log::error("Mesh %d added!", size);
	meshes.push_back(new Graphics::IndexedMesh(s));
	return VisualData{size, s.uvs != nullptr, s.normals != nullptr};
}

VisualData registerMeshFor(const ShapeClass* shapeClass, const Graphics::VisualShape& mesh) {
	if(shapeClassMeshIds.find(shapeClass) != shapeClassMeshIds.end()) throw "Attempting to re-register existing ShapeClass!";

	VisualData meshData = addMeshShape(mesh);

	//Log::error("Mesh %d registered!", meshData);

	shapeClassMeshIds.insert(std::pair<const ShapeClass*, VisualData>(shapeClass, meshData));
	return meshData;
}

VisualData registerMeshFor(const ShapeClass* shapeClass) {
	return registerMeshFor(shapeClass, Graphics::VisualShape(shapeClass->asPolyhedron()));
}

VisualData getOrCreateMeshFor(const ShapeClass* shapeClass) {
	auto found = shapeClassMeshIds.find(shapeClass);

	if(found != shapeClassMeshIds.end()) {
		// mesh found!
		VisualData meshData = (*found).second;
		//Log::error("Mesh %d reused!", meshData);
		return meshData;
	} else {
		// mesh not found :(
		return registerMeshFor(shapeClass);
	}
}


};
