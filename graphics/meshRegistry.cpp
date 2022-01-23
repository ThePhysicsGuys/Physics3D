#include "core.h"
#include "meshRegistry.h"

#include <Physics3D/math/constants.h>
#include <Physics3D/geometry/shapeLibrary.h>
#include <Physics3D/geometry/builtinShapeClasses.h>

#include "mesh/indexedMesh.h"

#include <stdexcept>
#include <cmath>
#include <cstddef>

namespace P3D::Graphics::MeshRegistry {

std::vector<IndexedMesh*> meshes;
std::map<const ShapeClass*, Comp::Mesh> shapeClassMeshIds;

Comp::Mesh box;
Comp::Mesh sphere;
Comp::Mesh cylinder;
Comp::Mesh wedge;
Comp::Mesh corner;
Comp::Mesh hexagon;

ExtendedTriangleMesh createCylinder(int sides, double radius, double height) {
	if (sides < 2)
		throw std::logic_error("Cannot create cylinder with <2 sides");

	int vertexCount = sides * 4;
	Vec3f* vertexBuffer = new Vec3f[vertexCount];

	float r = static_cast<float>(radius);
	float h = static_cast<float>(height);

	// vertices
	for (int i = 0; i < sides; i++) {
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
	for (int i = 0; i < sides; i++) {
		int bottomLeft = i * 2;
		int bottomRight = ((i + 1) % sides) * 2;
		triangleBuffer[i * 2] = Triangle{bottomLeft, bottomLeft + 1, bottomRight}; // botLeft, botRight, topLeft
		triangleBuffer[i * 2 + 1] = Triangle{bottomRight + 1, bottomRight, bottomLeft + 1}; // topRight, topLeft, botRight
	}

	Triangle* capOffset = triangleBuffer + static_cast<std::size_t>(sides) * 2;
	// top and bottom
	for (int i = 0; i < sides - 2; i++) {
		// common corner is i=0
		capOffset[i] = Triangle{sides * 2 + 0, sides * 2 + (i + 1) * 2, sides * 2 + (i + 2) * 2};
		capOffset[i + (sides - 2)] = Triangle{sides * 2 + 1, sides * 2 + (i + 2) * 2 + 1, sides * 2 + (i + 1) * 2 + 1};
	}

	Vec3f* normalBuffer = new Vec3f[vertexCount];
	for (int i = 0; i < sides * 2; i++) {
		Vec3f vertex = vertexBuffer[i];
		normalBuffer[i] = normalize(Vec3(vertex.x, vertex.y, 0));
	}

	for (int i = 0; i < sides; i++) {
		normalBuffer[i * 2 + sides * 2] = Vec3f(0, 0, 1);
		normalBuffer[i * 2 + sides * 2 + 1] = Vec3f(0, 0, -1);
	}

	Vec2f* uvBuffer = new Vec2f[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3f& p = vertexBuffer[i];

		float u;
		float v = p.y / h;
		if (p.x >= 0) {
			if (p.x == 0) {
				u = std::asin(p.y / std::sqrt(p.x * p.x + p.y * p.y));
			} else {
				u = std::atan2(p.y, p.x);
			}
		} else {
			u = -std::asin(p.y / std::sqrt(p.x * p.x + p.y * p.y)) + PI;
		}

		uvBuffer[i] = Vec2f(u, v);
	}

	ExtendedTriangleMesh cylinderShape(vertexBuffer, vertexCount, triangleBuffer, triangleCount);
	cylinderShape.setNormalBuffer(SharedArrayPtr<const Vec3f>(normalBuffer));
	cylinderShape.setUVBuffer(SharedArrayPtr<const Vec2f>(uvBuffer));

	return cylinderShape;
}

ExtendedTriangleMesh createSphere(double radius, int steps) {
	Polyhedron sphere(ShapeLibrary::createSphere(static_cast<float>(radius), steps));
	ExtendedTriangleMesh sphereShape(sphere);

	int i = 0;
	Vec3f* normalBuffer = new Vec3f[sphereShape.vertexCount];
	Vec2f* uvBuffer = new Vec2f[sphereShape.vertexCount];
	for (Vec3f vertex : sphereShape.iterVertices()) {
		Vec3f normal = normalize(vertex);
		Vec2f uv = Vec2f(atan2(normal.x, normal.z) / (2 * PI) + 0.5, normal.y * 0.5 + 0.5);

		normalBuffer[i] = normal;
		uvBuffer[i] = uv;

		i++;
	}

	sphereShape.setNormalBuffer(SharedArrayPtr<const Vec3f>(normalBuffer));
	sphereShape.setUVBuffer(SharedArrayPtr<const Vec2f>(uvBuffer));

	return sphereShape;
}

ExtendedTriangleMesh createBox(float width, float height, float depth) {
	Polyhedron box(ShapeLibrary::createBox(width, height, depth));
	ExtendedTriangleMesh boxShape = ExtendedTriangleMesh(box);

	Vec3f* normalBuffer = new Vec3f[boxShape.vertexCount];
	box.computeNormals(normalBuffer);

	Vec2f* uvBuffer = new Vec2f[boxShape.triangleCount * 3];
	for (std::size_t ti = 0; ti < boxShape.triangleCount; ti++) {
		Triangle t = boxShape.getTriangle(ti);
		Vec3f v[3];
		v[0] = boxShape.getVertex(t.firstIndex);
		v[1] = boxShape.getVertex(t.secondIndex);
		v[2] = boxShape.getVertex(t.thirdIndex);

		Vec3f normalVec = boxShape.getNormalVecOfTriangle(t);

		int side = getAbsMaxElementIndex(normalVec);
		Vec3f sizes{width, height, depth};

		for (std::size_t i = 0; i < 3; i++) {
			Vec2f vec = withoutIndex(v[i], side);
			Vec2f dim = withoutIndex(sizes, side);
			vec.x = (vec.x + dim.x / 2.0) / dim.x;
			vec.y = (vec.y + dim.y / 2.0) / dim.y;
			uvBuffer[ti * 3 + i] = vec;
		}
	}

	boxShape.setNormalBuffer(SharedArrayPtr<const Vec3f>(normalBuffer));
	boxShape.setUVBuffer(SharedArrayPtr<const Vec2f>(uvBuffer));

	return boxShape;
}

ExtendedTriangleMesh createCube(float size) {
	return createBox(size, size, size);
}

ExtendedTriangleMesh createHexagon(float radius, float height) {
	ExtendedTriangleMesh hexagonMesh(ShapeLibrary::createPrism(6, radius, height));

	return hexagonMesh;
}

void init() {
	sphere = registerShapeClass(&SphereClass::instance, ExtendedTriangleMesh::generateSmoothNormalsShape(SphereClass::instance.asPolyhedron()));
	cylinder = registerShapeClass(&CylinderClass::instance, createCylinder(64, 1.0, 2.0));
	box = registerShapeClass(&CubeClass::instance);
	wedge = registerShapeClass(&WedgeClass::instance);
	corner = registerShapeClass(&CornerClass::instance);
	hexagon = registerShape(createHexagon(0.5, 1.0));
}

Comp::Mesh registerShape(const ExtendedTriangleMesh& mesh) {
	std::size_t id = meshes.size();

	meshes.push_back(new IndexedMesh(mesh));

	return Comp::Mesh(id, mesh.getFlags());
}

Comp::Mesh registerShapeClass(const ShapeClass* shapeClass, const ExtendedTriangleMesh& mesh) {
	Comp::Mesh meshData = registerShape(mesh);

	auto iterator = shapeClassMeshIds.find(shapeClass);
	if (iterator != shapeClassMeshIds.end()) 
		iterator->second = meshData;
	else 
		shapeClassMeshIds.emplace(shapeClass, meshData);

	return meshData;
}

Comp::Mesh registerShapeClass(const ShapeClass* shapeClass) {
	auto iterator = shapeClassMeshIds.find(shapeClass);
	if (iterator != shapeClassMeshIds.end())
		return iterator->second;

	ExtendedTriangleMesh shape = ExtendedTriangleMesh::generateSplitNormalsShape(shapeClass->asPolyhedron());
	return registerShapeClass(shapeClass, shape);
}

Comp::Mesh getMesh(const ShapeClass* shapeClass) {
	auto iterator = shapeClassMeshIds.find(shapeClass);

	if (iterator != shapeClassMeshIds.end())
		return iterator->second;
	else
		return registerShapeClass(shapeClass);
}

std::size_t getID(const ShapeClass* shapeClass) {
	auto iterator = shapeClassMeshIds.find(shapeClass);

	if (iterator != shapeClassMeshIds.end())
		return iterator->second.id;
	else
		return registerShapeClass(shapeClass).id;
}

IndexedMesh* get(std::size_t id) {
	return meshes[id];
}

IndexedMesh* get(const Comp::Mesh& mesh) {
	if (mesh.id >= meshes.size())
		return nullptr;

	return get(mesh.id);
}

IndexedMesh* get(const ShapeClass* shapeClass) {
	return get(getID(shapeClass));
}

};
