#include "generators.h"

#include <vector>

#include "../physics/hardconstraints/motorConstraint.h"
#include "../physics/hardconstraints/sinusoidalPistonConstraint.h"

#include "../physics/geometry/convexShapeBuilder.h"
#include "../physics/misc/toString.h"

static std::default_random_engine generator;

int generateInt(int max) {
	return std::uniform_int_distribution<int>(0, max - 1)(generator);
}
size_t generateSize_t(size_t max) {
	return std::uniform_int_distribution<size_t>(0, max - 1)(generator);
}
double generateDouble() {
	return std::uniform_real_distribution<double>(0.0, 2.0)(generator);
}
float generateFloat() {
	return std::uniform_real_distribution<float>(0.0f, 2.0f)(generator);
}
double generateDouble(double min, double max) {
	return std::uniform_real_distribution<double>(min, max)(generator);
}
float generateFloat(float min, float max) {
	return std::uniform_real_distribution<float>(min, max)(generator);
}

bool generateBool() {
	return std::uniform_int_distribution<int>(0, 1)(generator) == 1;
}

Shape generateShape() {
	return boxShape(generateDouble(), generateDouble(), generateDouble());
}

Polyhedron generateConvexPolyhedron() {
	constexpr size_t MAX_POINT_COINT = 50;
	Vec3f vecBuf[MAX_POINT_COINT * 10]{generateVec3f(), generateVec3f(), generateVec3f(), generateVec3f()};
	if((vecBuf[1] - vecBuf[0]) % (vecBuf[2] - vecBuf[0]) * vecBuf[3] > 0) {
		for(int i = 0; i < 4; i++) {
			vecBuf[i].z = -vecBuf[i].z;
		}
	}
	Triangle triangleBuf[MAX_POINT_COINT*2 * 10]{{0,1,2}, {0,2,3}, {0,3,1}, {3,2,1}};
	TriangleNeighbors neighborBuf[MAX_POINT_COINT * 2 * 10];
	int removalBuf[MAX_POINT_COINT * 2 * 10];
	EdgePiece edgeBuf[MAX_POINT_COINT * 4 * 10];

	ConvexShapeBuilder builder(vecBuf, triangleBuf, 4, 4, neighborBuf, removalBuf, edgeBuf);

	int numExtraPoints = generateInt(MAX_POINT_COINT - 4);
	for(int i = 0; i < numExtraPoints; i++) {
		builder.addPoint(generateVec3f());
	}

	return builder.toPolyhedron();
}

static Triangle finishTriangle(int maxIndex, int firstIndex) {
	int secondIndex, thirdIndex;

	do {
		secondIndex = generateInt(maxIndex);
	} while(secondIndex == firstIndex);

	do {
		thirdIndex = generateInt(maxIndex);
	} while(thirdIndex == firstIndex || thirdIndex == secondIndex);

	return Triangle{firstIndex, secondIndex, thirdIndex};
}

Triangle generateTriangle(int maxIndex) {
	int firstIndex = generateInt(maxIndex);

	return finishTriangle(maxIndex, firstIndex);
}

TriangleMesh generateTriangleMesh() {
	int numVertices = generateInt(46) + 4;
	int numTriangles = generateInt(100) + numVertices;
	EditableMesh mesh(numVertices, numTriangles);

	for(int i = 0; i < numVertices; i++) {
		mesh.setVertex(i, generateVec3f());
	}

	for(int i = 0; i < numVertices; i++) {
		mesh.setTriangle(i, finishTriangle(numVertices, i)); // ensure one triangle per vertex
	}

	for(int i = numVertices; i < numTriangles; i++) {
		mesh.setTriangle(i, generateTriangle(numVertices)); // extra triangles
	}
	
	return TriangleMesh(std::move(mesh));
}

PositionTemplate<float> generatePositionf() {
	return PositionTemplate<float>(generateFloat(), generateFloat(), generateFloat());
}

Position generatePosition() {
	return Position(generateDouble(), generateDouble(), generateDouble());
}

BoundsTemplate<float> generateBoundsf() {
	PositionTemplate<float> a = generatePositionf();
	PositionTemplate<float> b = generatePositionf();

	return BoundsTemplate<float>(PositionTemplate<float>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)), PositionTemplate<float>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)));
}

Bounds generateBounds() {
	Position a = generatePosition();
	Position b = generatePosition();

	return Bounds(Position(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)), Position(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)));
}

Rotation generateRotation() {
	return Rotation::fromEulerAngles(generateDouble() * 1.507075, generateDouble() * 1.507075, generateDouble() * 1.507075);
}

CFrame generateCFrame() {
	return CFrame(generateVec3(), generateRotation());
}

GlobalCFrame generateGlobalCFrame() {
	return GlobalCFrame(generatePosition(), generateRotation());
}


TranslationalMotion generateTranslationalMotion() {
	return TranslationalMotion(generateTaylor<2>(generateVec3));
}
RotationalMotion generateRotationalMotion() {
	return RotationalMotion(generateTaylor<2>(generateVec3));
}
Motion generateMotion() {
	return Motion(generateTranslationalMotion(), generateRotationalMotion());
}
RelativeMotion generateRelativeMotion() {
	return RelativeMotion(generateMotion(), generateCFrame());
}


PartProperties generatePartProperties() {
	return PartProperties{generateDouble(),generateDouble(),generateDouble()};
}
Part generatePart() {
	return Part(generateShape(), generateGlobalCFrame(), generatePartProperties());
}

Part generatePart(Part& attachTo) {
	return Part(generateShape(), attachTo, generateCFrame(), generatePartProperties());
}

Part generatePart(Part& attachTo, HardConstraint* constraint) {
	return Part(generateShape(), attachTo, constraint, generateCFrame(), generateCFrame(), generatePartProperties());
}

HardConstraint* generateHardConstraint() {
	return new MotorConstraintTemplate<ConstantMotorTurner>(generateDouble());
}

void generateAttachment(Part& first, Part& second) {
	if(generateBool()) {
		first.attach(&second, generateCFrame());
	} else {
		first.attach(&second, generateHardConstraint(), generateCFrame(), generateCFrame());
	}
}

std::vector<Part> generateMotorizedPhysicalParts() {
	//int size = int(std::sqrt(rand() % 1000)) + 1;

	int size = rand() % 5 + 1;
	std::vector<Part> parts(size);
	parts[0] = generatePart();
	if(generateBool()) parts[0].ensureHasParent();

	for(int i = 1; i < size; i++) {
		parts[i] = generatePart();
		generateAttachment(parts[i], parts[rand() % i]);
	}

	return parts;
}

using namespace P3D::OldBoundsTree;

void generateLayerAssignment(std::vector<Part>& parts, WorldPrototype& world) {
	std::vector<Part*> layerParts(world.layers.size(), nullptr);

	int selectedSubLayer = rand() % ColissionLayer::NUMBER_OF_SUBLAYERS;
	for(Part& curPart : parts) {
		int selectedLayer = rand() % world.layers.size();
		WorldLayer& addTo = world.layers[selectedLayer].subLayers[selectedSubLayer];
		if(layerParts[selectedLayer] == nullptr) {
			addTo.tree.add(&curPart);
			layerParts[selectedLayer] = &curPart;
		} else {
			addTo.tree.addToGroup(&curPart, layerParts[selectedLayer]);
		}
		curPart.layer = &addTo;
	}
}

TreeNode generateTreeNode(int branchInhibition) {
	if(rand() % branchInhibition == 0) {
		// generate branching node
		int branches = (rand() % (MAX_BRANCHES - 2) + 2); // generate >=2 branches
		TreeNode result = TreeNode::withEmptySubNodes();
		result.nodeCount = branches;
		for(int i = 0; i < branches; i++) {
			result[i] = generateTreeNode(branchInhibition + 1); // to generate reasonably sized trees but not too big
		}
		result.recalculateBoundsFromSubBounds();
		return result;
	} else {
		// generate leaf node
		BasicBounded* newBasicBounded = new BasicBounded{generateBounds()};
		return TreeNode(newBasicBounded, newBasicBounded->getBounds());
	}
}
void assignGroupHeads(TreeNode& curNode) {
	if(curNode.isLeafNode() || rand() % 3 == 0) {
		curNode.isGroupHead = true;
	} else {
		for(TreeNode& subNode : curNode) {
			assignGroupHeads(subNode);
		}
	}
}

BoundsTree<BasicBounded> generateFilledBoundsTree() {
	TreeNode rootNode = generateTreeNode(2);
	assignGroupHeads(rootNode);
	BoundsTree<BasicBounded> result;
	result.rootNode = std::move(rootNode);
	return result;
}

BoundsTree<BasicBounded> generateBoundsTree() {
	if(rand() % 10 == 0) {
		return BoundsTree<BasicBounded>();
	} else {
		return generateFilledBoundsTree();
	}
}

void* getRandomLeafObject(const TreeNode& node) {
	if(node.isLeafNode()) {
		return node.object;
	} else {
		return getRandomLeafObject(node.subTrees[rand() % node.nodeCount]);
	}
}
