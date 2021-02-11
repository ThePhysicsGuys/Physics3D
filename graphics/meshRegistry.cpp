#include "core.h"
#include "meshRegistry.h"

#include "../physics/math/constants.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/builtinShapeClasses.h"

#include "buffers/bufferLayout.h"
#include "buffers/vertexBuffer.h"
#include "mesh/indexedMesh.h"

#include <stdexcept>
#include <cmath>
#include <cstddef>

namespace P3D::Graphics::MeshRegistry {

std::vector<IndexedMesh*> meshes;
std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
VisualData box;
VisualData sphere;
VisualData cylinder;

// Generates a cylinder with 
VisualShape createCylinder(int sides, double radius, double height) {
	if(sides < 2) { throw std::logic_error("Cannot create cylinder with <2 sides"); }
	int vertexCount = sides * 4;
	Vec3f* vertexBuffer = new Vec3f[vertexCount];

	float r = static_cast<float>(radius);
	float h = static_cast<float>(height);

	// vertices
	for(int i = 0; i < sides; i++) {
		float angle = i * pi<float>() * 2 / sides;
		Vec3f bottom(std::cos(angle) * r, std::sin(angle) * r, h / 2);
		Vec3f top(std::cos(angle) * r, std::sin(angle) * r, -h / 2);
		vertexBuffer[i * 2] = bottom;
		vertexBuffer[i * 2 + 1] = top;
		vertexBuffer[i * 2 + sides * 2] = bottom;
		vertexBuffer[i * 2 + 1 + sides * 2] = top;
	}

	int triangleCount = sides * 2 + (sides - 2) * 2;
	Triangle* triangleBuffer = new Triangle[triangleCount];

	// sides
	for(int i = 0; i < sides; i++) {
		int bottomLeft = i * 2;
		int bottomRight = ((i + 1) % sides) * 2;
		triangleBuffer[i * 2] = Triangle{bottomLeft, bottomLeft + 1, bottomRight}; // botLeft, botRight, topLeft
		triangleBuffer[i * 2 + 1] = Triangle{bottomRight + 1, bottomRight, bottomLeft + 1}; // topRight, topLeft, botRight
	}

	Triangle* capOffset = triangleBuffer + static_cast<std::size_t>(sides) * 2;
	// top and bottom
	for(int i = 0; i < sides - 2; i++) { // common corner is i=0
		capOffset[i] = Triangle{sides * 2 + 0, sides * 2 + (i + 1) * 2, sides * 2 + (i + 2) * 2};
		capOffset[i + (sides - 2)] = Triangle{sides * 2 + 1, sides * 2 + (i + 2) * 2 + 1, sides * 2 + (i + 1) * 2 + 1};
	}

	Vec3f* normalBuffer = new Vec3f[vertexCount];
	for (int i = 0; i < sides * 2; i++) {
		Vec3f vertex = vertexBuffer[i];
		normalBuffer[i] = normalize(Vec3(vertex.x, vertex.y, 0));
	}

	for(int i = 0; i < sides; i++) {
		normalBuffer[i * 2 + sides * 2] = Vec3f(0, 0, 1);
		normalBuffer[i * 2 + sides * 2 + 1] = Vec3f(0, 0, -1);
	}

	Vec2f* uvBuffer = new Vec2f[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3f& p = vertexBuffer[i];

		float u;
		float v = p.y / h;
		if (p.x >= 0) {
			if(p.x == 0) {
				u = std::asin(p.y / std::sqrt(p.x * p.x + p.y * p.y));
			} else {
				u = std::atan2(p.y, p.x);
			}
		} else {
			u = -std::asin(p.y / std::sqrt(p.x * p.x + p.y * p.y)) + PI;
		}

		uvBuffer[i] = Vec2f(u, v);
	}

	return VisualShape(vertexBuffer, vertexCount, triangleBuffer, triangleCount, SharedArrayPtr<const Vec3f>(normalBuffer), SharedArrayPtr<const Vec2f>(uvBuffer));
}

VisualShape createSphere(double radius, int steps) {
	Polyhedron sphere(Library::createSphere(static_cast<float>(radius), steps));
	VisualShape sphereShape = VisualShape(sphere);

	Vec3f* normalBuffer = new Vec3f[sphereShape.vertexCount];
	Vec2f* uvBuffer = new Vec2f[sphereShape.vertexCount];
	int i = 0;
	for (Vec3f vertex : sphereShape.iterVertices()) {
		Vec3f normal = normalize(vertex);
		Vec2f uv = Vec2f(atan2(normal.x, normal.z) / (2 * PI) + 0.5, normal.y * 0.5 + 0.5);

		normalBuffer[i] = normal;
		uvBuffer[i] = uv;

		i++;
	}

	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuffer);
	sphereShape.uvs = SharedArrayPtr<const Vec2f>(uvBuffer);
	return sphereShape;
}

VisualShape createBox(float width, float height, float depth) {
	Polyhedron box(Library::createBox(width, height, depth));
	VisualShape boxShape = VisualShape(box);

	Vec3f* normalBuffer = new Vec3f[boxShape.vertexCount];
	box.computeNormals(normalBuffer);

	boxShape.normals = SharedArrayPtr<const Vec3f>(normalBuffer);
	
	Vec2f* uvBuffer = new Vec2f[boxShape.triangleCount * 3];

	for(std::size_t ti = 0; ti < boxShape.triangleCount; ti++) {
		Triangle t = boxShape.getTriangle(ti);
		Vec3f v[3];
		v[0] = boxShape.getVertex(t.firstIndex);
		v[1] = boxShape.getVertex(t.secondIndex);
		v[2] = boxShape.getVertex(t.thirdIndex);

		Vec3f normalVec = boxShape.getNormalVecOfTriangle(t);

		int side = getAbsMaxElementIndex(normalVec);
		Vec3f sizes { width, height, depth };
		
		for(std::size_t i = 0; i < 3; i++) {
			Vec2f vec = withoutIndex(v[i], side);
			Vec2f dim = withoutIndex(sizes, side);
			 vec.x = (vec.x + dim.x / 2.0) / dim.x;
			 vec.y = (vec.y + dim.y / 2.0) / dim.y;
			 uvBuffer[ti * 3 + i] = vec;
		}
	}

	boxShape.uvs = SharedArrayPtr<const Vec2f>(uvBuffer);
	return boxShape;
}

VisualShape createCube(float size) {
	return createBox(size, size, size);
}

void init() {
	sphere = registerMeshFor(&SphereClass::instance, VisualShape::generateSmoothNormalsShape(SphereClass::instance.asPolyhedron()));
	box = registerMeshFor(&CubeClass::instance);
	VisualShape cylinderShapeMesh = createCylinder(64, 1.0, 2.0);
	cylinder = registerMeshFor(&CylinderClass::instance, cylinderShapeMesh);
}


VisualData addMeshShape(const VisualShape& shape) {
	std::size_t size = meshes.size();
	
	IndexedMesh* mesh = new IndexedMesh(shape);
	meshes.push_back(mesh);

	return VisualData { size, shape.uvs != nullptr, shape.normals != nullptr };
}

VisualData registerMeshFor(const ShapeClass* shapeClass, const Graphics::VisualShape& mesh) {
	if(shapeClassMeshIds.find(shapeClass) != shapeClassMeshIds.end()) throw "Attempting to re-register existing ShapeClass!";

	VisualData meshData = addMeshShape(mesh);

	//Log::error("Mesh %d registered!", meshData);

	shapeClassMeshIds.insert(std::pair<const ShapeClass*, VisualData>(shapeClass, meshData));
	return meshData;
}

VisualData registerMeshFor(const ShapeClass* shapeClass) {
	VisualShape shape = VisualShape::generateSplitNormalsShape(shapeClass->asPolyhedron());
	return registerMeshFor(shapeClass, shape);
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
