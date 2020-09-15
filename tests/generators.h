#pragma once

#include <tuple>

#include "../physics/math/linalg/vec.h"
#include "../physics/math/linalg/mat.h"
#include "../physics/math/rotation.h"
#include "../physics/math/cframe.h"
#include "../physics/math/globalCFrame.h"
#include "../physics/math/position.h"
#include "../physics/math/bounds.h"
#include "../physics/math/fix.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/geometry/polyhedron.h"

#include "../physics/part.h"
#include "../physics/physical.h"
#include "../physics/constraints/hardConstraint.h"
#include "../physics/layer.h"

#include "../physics/datastructures/boundsTree.h"

struct BasicBounded {
	Bounds bounds;
	inline Bounds getBounds() const { return bounds; }
};

double generateDouble();
bool generateBool();
Shape generateShape();
Vec3 generateVec3();
Position generatePosition();
Bounds generateBounds();
Rotation generateRotation();
CFrame generateCFrame();
GlobalCFrame generateGlobalCFrame();
PartProperties generatePartProperties();
Part generatePart();
Part generatePart(Part& attachTo);
Part generatePart(Part& attachTo, HardConstraint* constraint);
HardConstraint* generateHardConstraint();
void generateAttachment(Part& first, Part& second);
std::tuple<MotorizedPhysical*, Part*, int> generateMotorizedPhysical();
void generateLayerAssignment(Part* parts, int partCount, WorldLayer* layers, int layerCount);
TreeNode generateTreeNode(int branchInhibition);
BoundsTree<BasicBounded> generateFilledBoundsTree();
BoundsTree<BasicBounded> generateBoundsTree();
void* getRandomLeafObject(const TreeNode& node);
template<typename Boundable>
Boundable* getRandomObjectFromTree(const BoundsTree<Boundable>& tree) {
	return static_cast<Boundable*>(getRandomLeafObject(tree.rootNode));
}
