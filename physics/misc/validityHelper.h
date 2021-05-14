#pragma once

#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/linalg/largeMatrix.h"
#include "../math/cframe.h"
#include "../motion.h"
#include "../datastructures/boundsTree.h"

#include <cmath>

namespace P3D {
#ifdef _MSC_VER
#define DEBUGBREAK __debugbreak()
#else
#define DEBUGBREAK
#endif

inline bool isValid(double d) {
	return std::isfinite(d) && std::abs(d) < 100000.0;
}
inline bool isValid(float f) {
	return std::isfinite(f) && std::abs(f) < 100000.0; // sanity check
}

template<typename T, size_t Size>
inline bool isVecValid(const Vector<T, Size>& vec) {
	for(size_t i = 0; i < Size; i++) {
		if(!isValid(vec[i])) return false;
	}
	return true;
}

template<typename T, size_t Height, size_t Width>
inline bool isMatValid(const Matrix<T, Height, Width>& mat) {
	for(size_t row = 0; row < Height; row++) {
		for(size_t col = 0; col < Width; col++) {
			if(!isValid(mat(row, col))) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const SymmetricMatrix<T, Size>& mat) {
	for(size_t row = 0; row < Size; row++) {
		for(size_t col = row; col < Size; col++) {
			if(!isValid(mat(row, col))) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const DiagonalMatrix<T, Size>& mat) {
	for(size_t i = 0; i < Size; i++) {
		if(!isValid(mat[i])) return false;
	}
	return true;
}

template<typename T>
inline bool isVecValid(const UnmanagedLargeVector<T>& vec) {
	for(size_t i = 0; i < vec.size(); i++) {
		if(!isValid(vec[i])) return false;
	}
	return true;
}

template<typename T>
inline bool isMatValid(const UnmanagedLargeMatrix<T>& mat) {
	for(size_t row = 0; row < mat.height(); row++) {
		for(size_t col = 0; col < mat.width(); col++) {
			if(!isValid(mat(row, col))) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const UnmanagedHorizontalFixedMatrix<T, Size>& mat) {
	for(size_t row = 0; row < mat.height(); row++) {
		for(size_t col = 0; col < mat.width(); col++) {
			if(!isValid(mat(row, col))) return false;
		}
	}
	return true;
}

template<typename T, size_t Size>
inline bool isMatValid(const UnmanagedVerticalFixedMatrix<T, Size>& mat) {
	for(size_t row = 0; row < mat.height(); row++) {
		for(size_t col = 0; col < mat.width(); col++) {
			if(!isValid(mat(row, col))) return false;
		}
	}
	return true;
}

template<typename T>
inline bool isRotationValid(const MatrixRotationTemplate<T>& rotation) {
	SquareMatrix<T, 3> rotMat = rotation.asRotationMatrix();
	return isValidRotationMatrix(rotMat);
}

template<typename T>
inline bool isRotationValid(const QuaternionRotationTemplate<T>& rotation) {
	Quaternion<T> rotQuat = rotation.asRotationQuaternion();
	return isValidRotationQuaternion(rotQuat);
}

template<typename T>
inline bool isCFrameValid(const CFrameTemplate<T>& cframe) {
	return isVecValid(cframe.getPosition()) && isRotationValid(cframe.getRotation());
}

template<typename T, std::size_t DerivationCount>
inline bool isTaylorExpansionValid(const TaylorExpansion<T, DerivationCount>& taylor) {
	for(Vec3 v : taylor) {
		if(!isVecValid(v)) return false;
	}
	return true;
}

inline bool isTranslationalMotionValid(const TranslationalMotion& motion) {
	return isTaylorExpansionValid(motion.translation);
}
inline bool isRotationalMotionValid(const RotationalMotion& motion) {
	return isTaylorExpansionValid(motion.rotation);
}
inline bool isMotionValid(const Motion& motion) {
	return isTranslationalMotionValid(motion.translation) && isRotationalMotionValid(motion.rotation);
}

struct Triangle;
class TriangleMesh;
class Polyhedron;
struct IndexedShape;

bool isValid(const TriangleMesh& mesh);
bool isValid(const Polyhedron& poly);
bool isValid(const IndexedShape& shape);

bool isValidTriangle(Triangle t, int vertexCount);

class Part;

void treeValidCheck(const OldBoundsTree::TreeNode& rootNode);
template<typename Boundable>
inline void treeValidCheck(const OldBoundsTree::BoundsTree<Boundable>& tree) {
	if(!tree.isEmpty()) {
		treeValidCheck(tree.rootNode);
	}
}

class MotorizedPhysical;

bool isMotorizedPhysicalValid(const MotorizedPhysical* mainPhys);
};


namespace P3D::NewBoundsTree {

template<typename Boundable>
inline bool isBoundsTreeValidRecursive(const TreeTrunk& curNode, int curNodeSize, int depth = 0) {
	for(int i = 0; i < curNodeSize; i++) {
		const TreeNodeRef& subNode = curNode.subNodes[i];

		BoundsTemplate<float> foundBounds = curNode.getBoundsOfSubNode(i);

		if(subNode.isTrunkNode()) {
			const TreeTrunk& subTrunk = subNode.asTrunk();
			int subTrunkSize = subNode.getTrunkSize();

			BoundsTemplate<float> realBounds = TrunkSIMDHelperFallback::getTotalBounds(subTrunk, subTrunkSize);

			if(realBounds != foundBounds) {
				std::cout << "(" << i << "/" << curNodeSize << ") Trunk bounds not up to date\n";
				return false;
			}

			if(!isBoundsTreeValidRecursive<Boundable>(subTrunk, subTrunkSize, depth + 1)) {
				std::cout << "(" << i << "/" << curNodeSize << ")\n";
				return false;
			}
		} else {
			const Boundable* itemB = static_cast<const Boundable*>(subNode.asObject());
			if(foundBounds != itemB->getBounds()) {
				std::cout << "(" << i << "/" << curNodeSize << ") Leaf not up to date\n";
				return false;
			}
		}
	}
	return true;
}

template<typename Boundable>
bool isBoundsTreeValid(const BoundsTreePrototype& tree) {
	std::pair<const TreeTrunk&, int> baseTrunk = tree.getBaseTrunk();
	return isBoundsTreeValidRecursive<Boundable>(baseTrunk.first, baseTrunk.second);
}

template<typename Boundable>
bool isBoundsTreeValid(const BoundsTree<Boundable>& tree) {
	return isBoundsTreeValid<Boundable>(tree.getPrototype());
}

template<typename Boundable>
inline void treeValidCheck(const NewBoundsTree::BoundsTree<Boundable>& tree) {
	if(!isBoundsTreeValid(tree)) throw "tree invalid!";
}

};
