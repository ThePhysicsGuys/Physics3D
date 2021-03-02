#pragma once

#include <random>
#include <utility>
#include <cstdint>

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/rotation.h"
#include "../physics/math/cframe.h"
#include "../physics/math/globalCFrame.h"
#include "../physics/math/position.h"
#include "../physics/math/bounds.h"
#include "../physics/math/fix.h"
#include "../physics/math/taylorExpansion.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/geometry/polyhedron.h"
#include "../physics/motion.h"
#include "../physics/relativeMotion.h"

#include "../physics/part.h"
#include "../physics/physical.h"
#include "../physics/hardconstraints/hardConstraint.h"
#include "../physics/layer.h"
#include "../physics/world.h"

#include "../physics/datastructures/boundsTree.h"

template<typename T>
T generate() {
	throw "Unsupported generate type!";
}

// generates between -2.0 and 2.0
template<>
inline double generate() {
	return rand() * 4.0 / RAND_MAX - 2.0;
}
// generates between -2.0 and 2.0
template<>
inline float generate() {
	return rand() * 4.0f / RAND_MAX - 2.0f;
}
template<>
inline int8_t generate() {
	return (rand() % 200) - 100;
}
template<>
inline uint8_t generate() {
	return uint8_t(rand()) & 0xFF;
}
template<>
inline int16_t generate() {
	return (rand() % 200) - 100;
}
template<>
inline uint16_t generate() {
	return rand() % 100;
}
template<>
inline int32_t generate() {
	return (rand() % 200) - 100;
}
template<>
inline uint32_t generate() {
	return rand() % 100;
}
template<>
inline int64_t generate() {
	return (rand() % 200) - 100;
}
template<>
inline uint64_t generate() {
	return rand() % 100;
}

int generateInt(int max);
size_t generateSize_t(size_t max);
double generateDouble(); 
float generateFloat();
bool generateBool();
Shape generateShape();
Polyhedron generateConvexPolyhedron();
TriangleMesh generateTriangleMesh();
template<typename T, std::size_t Size>
Vector<T, Size> generateVector() {
	Vector<T, Size> result;

	for(std::size_t i = 0; i < Size; i++) {
		result[i] = generate<T>();
	}

	return result;
}
template<typename T, std::size_t Height, std::size_t Width>
Matrix<T, Height, Width> generateMatrix() {
	Matrix<T, Height, Width> result;

	for(std::size_t i = 0; i < Height; i++) {
		for(std::size_t j = 0; j < Width; j++) {
			result(i, j) = generate<T>();
		}
	}

	return result;
}
inline Vec3 generateVec3() { return generateVector<double, 3>(); }
inline Vec3f generateVec3f() { return generateVector<float, 3>(); }
Triangle generateTriangle(int maxIndex);
Position generatePosition();
Bounds generateBounds();
BoundsTemplate<float> generateBoundsf();
PositionTemplate<float> generatePositionf();
Rotation generateRotation();
CFrame generateCFrame();
GlobalCFrame generateGlobalCFrame();
template<int Derivatives, typename ItemGenerator>
auto generateTaylor(ItemGenerator itemGenerator) -> TaylorExpansion<decltype(itemGenerator()), Derivatives> {
	TaylorExpansion<decltype(itemGenerator()), Derivatives> result;

	for(auto& item : result) {
		item = itemGenerator();
	}

	return result;
}
template<int Derivatives, typename ItemGenerator>
auto generateFullTaylor(ItemGenerator itemGenerator) -> FullTaylorExpansion<decltype(itemGenerator()), Derivatives> {
	FullTaylorExpansion<decltype(itemGenerator()), Derivatives> result;

	for(auto& item : result) {
		item = itemGenerator();
	}

	return result;
}
TranslationalMotion generateTranslationalMotion();
RotationalMotion generateRotationalMotion();
Motion generateMotion();
RelativeMotion generateRelativeMotion();
PartProperties generatePartProperties();
Part generatePart();
Part generatePart(Part& attachTo);
Part generatePart(Part& attachTo, HardConstraint* constraint);
HardConstraint* generateHardConstraint();
void generateAttachment(Part& first, Part& second);
std::vector<Part> generateMotorizedPhysicalParts();
void generateLayerAssignment(std::vector<Part>& parts, WorldPrototype& world);
TreeNode generateTreeNode(int branchInhibition);
BoundsTree<BasicBounded> generateFilledBoundsTree();
BoundsTree<BasicBounded> generateBoundsTree();
void* getRandomLeafObject(const TreeNode& node);
template<typename Boundable>
Boundable* getRandomObjectFromTree(const BoundsTree<Boundable>& tree) {
	return static_cast<Boundable*>(getRandomLeafObject(tree.rootNode));
}
template<typename Collection>
auto oneOf(const Collection& collection) -> decltype(collection[0]) {
	return collection[generateSize_t(collection.size())];
}
template<typename Collection>
auto oneOf(Collection& collection) -> decltype(collection[0]) {
	return collection[generateSize_t(collection.size())];
}
