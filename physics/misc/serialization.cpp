#include "serialization.h"
#include "../geometry/polyhedron.h"
#include "../geometry/polyhedronInternals.h"
#include "../geometry/shape.h"
#include "../geometry/shapeClass.h"
#include "../part.h"
#include "../world.h"

void serialize(const char* data, size_t size, std::ostream& ostream) {
	ostream.write(data, size);
}

void deserialize(char* buf, size_t size, std::istream& istream) {
	for(size_t i = 0; i < size; i++)
		buf[i] = istream.get();
}

template<>
void serialize<char>(const char& c, std::ostream& ostream) {
	ostream << c;
}
template<>
char deserialize<char>(std::istream& istream) {
	return istream.get();
}

template<>
void serialize<Polyhedron>(const Polyhedron& poly, std::ostream& ostream) {
	::serialize<int>(poly.vertexCount, ostream);
	::serialize<int>(poly.triangleCount, ostream);

	for(int i = 0; i < poly.vertexCount; i++) {
		::serialize<Vec3f>(poly[i], ostream);
	}
	for(int i = 0; i < poly.triangleCount; i++) {
		::serialize<Triangle>(poly.getTriangle(i), ostream);
	}
}
template<>
Polyhedron deserialize<Polyhedron>(std::istream& istream) {
	int vertexCount = ::deserialize<int>(istream);
	int triangleCount = ::deserialize<int>(istream);

	Vec3f* vertices = new Vec3f[vertexCount];
	Triangle* triangles = new Triangle[triangleCount];

	for(int i = 0; i < vertexCount; i++) {
		vertices[i] = ::deserialize<Vec3f>(istream);
	}
	for(int i = 0; i < triangleCount; i++) {
		triangles[i] = ::deserialize<Triangle>(istream);
	}

	Polyhedron result(vertices, triangles, vertexCount, triangleCount);
	delete[] vertices;
	delete[] triangles;
	return result;
}

template<>
void serialize(const Shape& shape, std::ostream& ostream) {
	::serialize<int>(shape.baseShape->classID, ostream);
	::serialize<double>(shape.getWidth(), ostream);
	::serialize<double>(shape.getHeight(), ostream);
	::serialize<double>(shape.getDepth(), ostream);
}
template<>
Shape deserialize(std::istream& istream) {
	const ShapeClass* shapeClass = ShapeClass::getShapeClassForId(::deserialize<int>(istream));
	double width = ::deserialize<double>(istream);
	double height = ::deserialize<double>(istream);
	double depth = ::deserialize<double>(istream);
	return Shape(shapeClass, width, height, depth);
}

template<>
void serialize<Part>(const Part& part, std::ostream& ostream) {
	::serialize<GlobalCFrame>(part.getCFrame(), ostream);
	::serialize(part.hitbox, ostream);
	::serialize<PartProperties>(part.properties, ostream);
}

template<>
Part deserialize<Part>(std::istream& istream) {
	GlobalCFrame cframe = ::deserialize<GlobalCFrame>(istream);
	Shape shape = ::deserialize<Shape>(istream);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cframe, properties);
}
