#pragma once

#include "math/linalg/vec.h"
#include "math/linalg/mat.h"
#include "math/cframe.h"
#include "math/bounds.h"
#include "math/position.h"
#include "math/globalCFrame.h"

#include "datastructures/unorderedVector.h"
#include "datastructures/iteratorEnd.h"
#include "datastructures/monotonicTree.h"

#include "part.h"
#include "rigidBody.h"
#include "constraints/hardConstraint.h"
#include "constraints/hardPhysicalConnection.h"

typedef Vec3 Vec3Local;
typedef Vec3 Vec3Relative;

class WorldPrototype;

class ConnectedPhysical;
class MotorizedPhysical;

class InternalMotionTree;
class NormalizedInternalMotionTree;

class Physical {
	void makeMainPart(AttachedPart& newMainPart);
protected:
	void updateAttachedPhysicals();
	void updateConstraints(double deltaT);
	void translateUnsafeRecursive(const Vec3Fix& translation);

	void setMainPhysicalRecursive(MotorizedPhysical* newMainPhysical);

	// deletes the given physical
	void attachPhysical(Physical* phys, const CFrame& attachment);
	// deletes the given physical
	void attachPhysical(MotorizedPhysical* phys, const CFrame& attachment);

	// deletes the given physical
	void attachPhysical(Physical* phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);

	// deletes the given physical
	void attachPhysical(MotorizedPhysical* phys, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);

	void removeChild(ConnectedPhysical* child);
	void detachFromRigidBody(Part* part);
	void detachFromRigidBody(AttachedPart&& part);

	void detachPartAssumingMultipleParts(Part* part);

	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachPartInChildren(const Func& func) const;

	// expects a function of type void(Part&)
	template<typename Func>
	void forEachPartInChildren(const Func& func);

	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachPartInThisAndChildren(const Func& func) const;

	// expects a function of type void(Part&)
	template<typename Func>
	void forEachPartInThisAndChildren(const Func& func);

	// expects a function of type void(const HardPhysicalConnection&, const Physical& parent, const Physical& child)
	template<typename Func>
	void forEachHardConstraintInChildren(const Func& func) const;

	// expects a function of type void(HardPhysicalConnection&, Physical& parent, Physical& child)
	template<typename Func>
	void forEachHardConstraintInChildren(const Func& func);
public:
	RigidBody rigidBody;

	MotorizedPhysical* mainPhysical;
	UnorderedVector<ConnectedPhysical> childPhysicals;

	Physical() = default;
	Physical(Part* mainPart, MotorizedPhysical* mainPhysical);
	Physical(RigidBody&& rigidBody, MotorizedPhysical* mainPhysical);

	Physical(Physical&& other) noexcept;
	Physical& operator=(Physical&& other) noexcept;
	Physical(const Physical&) = delete;
	void operator=(const Physical&) = delete;

	void setCFrame(const GlobalCFrame& newCFrame);
	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);
	inline const GlobalCFrame& getCFrame() const { return rigidBody.getCFrame(); }

	inline Position getPosition() const {
		return getCFrame().getPosition();
	}

	/*
		Returns the motion of this physical positioned at it's getCFrame()

		The motion is oriented globally
	*/
	Motion getMotion() const;
	/*
		Returns the motion of this physical positioned at it's center of mass

		The motion is oriented globally
	*/
	Motion getMotionOfCenterOfMass() const;

	double getVelocityKineticEnergy() const;
	double getAngularKineticEnergy() const;
	double getKineticEnergy() const;

	void applyDragToPhysical(Vec3 origin, Vec3 drag);
	void applyForceToPhysical(Vec3 origin, Vec3 force);
	void applyImpulseToPhysical(Vec3 origin, Vec3 impulse);

	CFrame getRelativeCFrameToMain() const;
	Vec3 localToMain(const Vec3Local& vec) const;

	void makeMainPart(Part* newMainPart);
	void attachPart(Part* part, const CFrame& attachment);
	/*
		detaches a part from this Physical
		Leaves the detached part with a MotorizedPhysical parent in which it is the only part
		As opposed to Physical::removePart(Part*)

		this Physical may no longer be valid after calling detachPart
	*/
	void detachPart(Part* part);
	/*
		removes a part from this Physical
		After this method: part->parent == nullptr
		As opposed to Physical::detachPart(Part*)

		this Physical may no longer be valid after calling removePart
	*/
	void removePart(Part* part);
	void attachPart(Part* part, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);

	size_t getNumberOfPhysicalsInThisAndChildren() const;
	size_t getNumberOfPartsInThisAndChildren() const;

	void notifyPartPropertiesChanged(Part* part);
	void notifyPartPropertiesAndBoundsChanged(Part* part, const Bounds& oldBounds);
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr);

	bool isValid() const;

	bool isMainPhysical() const;
	MotorizedPhysical* makeMainPhysical();

private:
	void detachAllChildPhysicals();
};


class ConnectedPhysical : public Physical {
	friend class Physical;
	friend class MotorizedPhysical;

	void refreshCFrame();
	void refreshCFrameRecursive();
public:
	HardPhysicalConnection connectionToParent;
	Physical* parent;

	ConnectedPhysical() = default;
	ConnectedPhysical(RigidBody&& rigidBody, Physical* parent, HardPhysicalConnection&& connectionToParent);
	ConnectedPhysical(Physical&& phys, Physical* parent, HardPhysicalConnection&& connectionToParent);
	ConnectedPhysical(Physical&& phys, Physical* parent, HardConstraint* constraintWithParent, const CFrame& attachOnThis, const CFrame& attachOnParent);
	
	void setCFrame(const GlobalCFrame& newCFrame);
	CFrame getRelativeCFrameToParent() const;


	/*
		Returns the motion of this physical positioned at it's getCFrame()

		The motion is oriented globally
	*/
	Motion getMotion() const;
	/*
		Returns the motion of this physical positioned at it's center of mass

		The motion is oriented globally
	*/
	Motion getMotionOfCenterOfMass() const;

	/*
		Makes this physical the main Physical
	*/
	void makeMainPhysical();

	RelativeMotion getRelativeMotionBetweenParentAndSelf() const;

	bool isValid() const;
};

class MotorizedPhysical : public Physical {
	friend class Physical;
	friend class ConnectedPhysical;
	void rotateAroundCenterOfMassUnsafe(const Rotation& rotation);
public:
	void refreshPhysicalProperties();
	Vec3 totalForce = Vec3(0.0, 0.0, 0.0);
	Vec3 totalMoment = Vec3(0.0, 0.0, 0.0);

	double totalMass;
	Vec3 totalCenterOfMass;

	WorldPrototype* world = nullptr;
	
	SymmetricMat3 forceResponse;
	SymmetricMat3 momentResponse;

	Motion motionOfCenterOfMass;
	
	explicit MotorizedPhysical(Part* mainPart);
	explicit MotorizedPhysical(RigidBody&& rigidBody);
	explicit MotorizedPhysical(Physical&& movedPhys);

	/*
		Returns the motion of this physical positioned at it's getCFrame()

		The motion is oriented globally
	*/
	Motion getMotion() const;
	/*
		Returns the motion of this physical positioned at it's center of mass

		The motion is oriented globally
	*/
	Motion getMotionOfCenterOfMass() const;

	Position getCenterOfMass() const;
	GlobalCFrame getCenterOfMassCFrame() const;

	Vec3 getTotalImpulse() const;
	Vec3 getTotalAngularMomentum() const;

	// expects a function of the form T(const T& parentValue, const ConnectedPhysical& conPhys)
	template<typename T, typename F>
	MonotonicTree<T> constructMonotonicTree(const T& rootNodeValue, F valueGenerator) const {
		std::size_t treeSize = this->getNumberOfPhysicalsInThisAndChildren();
		MonotonicTreeBuilder<T> builder(treeSize);
		MonotonicTreeNode<T>& rootNode = builder.getRootNode();
		rootNode.value = rootNodeValue;

		struct NestedRecurse {
			static void recurse(MonotonicTreeBuilder<T>& builder, MonotonicTreeNode<T>& currentNode, const Physical& currentPhys, F valueGenerator) {
				std::size_t childCount = currentPhys.childPhysicals.size();
				if(childCount > 0) {
					MonotonicTreeNode<RelativeMotion>* childNodes = builder.alloc(childCount);
					currentNode.children = childNodes;
					for(std::size_t i = 0; i < childCount; i++) {
						const ConnectedPhysical& conPhys = currentPhys.childPhysicals[i];
						MonotonicTreeNode<RelativeMotion>& currentChildNode = childNodes[i];
						currentChildNode.value = valueGenerator(currentNode.value, conPhys);
						recurse(builder, currentChildNode, conPhys, valueGenerator);
					}
				} else {
					currentNode.children = nullptr;
				}
			}
		};
		
		NestedRecurse::recurse(builder, rootNode, *this, valueGenerator);
		return MonotonicTree<T>(std::move(builder));
	}

	// connectedPhysFunc is of the form void(const T& currentValue, const ConnectedPhysical& currentPhysical)
	template<typename T, typename F>
	void mutuallyRecurse(const MonotonicTree<T>& tree, F connectedPhysFunc) const {
		struct NestedRecurse {
			static void recurse(const MonotonicTreeNode<T>& currentNode, const Physical& currentPhys, F func) {
				std::size_t childCount = currentPhys.childPhysicals.size();
				for(std::size_t i = 0; i < childCount; i++) {
					const ConnectedPhysical& conPhys = currentPhys.childPhysicals[i];
					MonotonicTreeNode<RelativeMotion>& currentChildNode = currentNode.children[i];
					func(currentChildNode, conPhys);

					recurse(currentChildNode, conPhys, func);
				}
			}
		};
		
		NestedRecurse::recurse(tree.getRootNode(), *this, connectedPhysFunc);
	}
	InternalMotionTree getInternalRelativeMotionTree() const;
	NormalizedInternalMotionTree getNormalizedInternalRelativeMotionTree() const;

	void ensureWorld(WorldPrototype* world);

	void update(double deltaT);

	void setCFrame(const GlobalCFrame& newCFrame);
	void rotateAroundCenterOfMass(const Rotation& rotation);
	void translate(const Vec3& translation);

	void applyForceAtCenterOfMass(Vec3 force);
	void applyForce(Vec3Relative origin, Vec3 force);
	void applyMoment(Vec3 moment);
	void applyImpulseAtCenterOfMass(Vec3 impulse);
	void applyImpulse(Vec3Relative origin, Vec3Relative impulse);
	void applyAngularImpulse(Vec3 angularImpulse);
	void applyDragAtCenterOfMass(Vec3 drag);
	void applyDrag(Vec3Relative origin, Vec3Relative drag);
	void applyAngularDrag(Vec3 angularDrag);

	SymmetricMat3 getResponseMatrix(const Vec3Local& localPoint) const;
	Mat3 getResponseMatrix(const Vec3Local& actionPoint, const Vec3Local& responsePoint) const;
	double getInertiaOfPointInDirectionLocal(const Vec3Local& localPoint, const Vec3Local& localDirection) const;
	double getInertiaOfPointInDirectionRelative(const Vec3Relative& relativePoint, const Vec3Relative& relativeDirection) const;
	inline Part* getMainPart() { return this->rigidBody.mainPart; }
	inline const Part* getMainPart() const { return this->rigidBody.mainPart; }

	void fullRefreshOfConnectedPhysicals();

	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachPart(const Func& func) const { this->forEachPartInThisAndChildren(func); }

	// expects a function of type void(Part&)
	template<typename Func>
	void forEachPart(const Func& func) { this->forEachPartInThisAndChildren(func); }

	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachPartExceptMainPart(const Func& func) const {
		this->rigidBody.forEachAttachedPart(func);
		this->forEachPartInChildren(func); 
	}

	// expects a function of type void(Part&)
	template<typename Func>
	void forEachPartExceptMainPart(const Func& func) {
		this->rigidBody.forEachAttachedPart(func);
		this->forEachPartInChildren(func); 
	}

	// expects a function of type void(const Physical& parent, const ConnectedPhysical& child)
	template<typename Func>
	void forEachHardConstraint(const Func& func) const {
		this->forEachHardConstraintInChildren(func);
	}

	// expects a function of type void(Physical& parent, ConnectedPhysical& child)
	template<typename Func>
	void forEachHardConstraint(const Func& func) {
		this->forEachHardConstraintInChildren(func);
	}

	bool isValid() const;
};

// expects a function of type void(const Part&)
template<typename Func>
void Physical::forEachPartInChildren(const Func& func) const {
	for(const ConnectedPhysical& conPhys : this->childPhysicals) {
		conPhys.forEachPartInThisAndChildren(func);
	}
}

// expects a function of type void(Part&)
template<typename Func>
void Physical::forEachPartInChildren(const Func& func) {
	for(ConnectedPhysical& conPhys : this->childPhysicals) {
		conPhys.forEachPartInThisAndChildren(func);
	}
}

// expects a function of type void(const Part&)
template<typename Func>
void Physical::forEachPartInThisAndChildren(const Func& func) const {
	this->rigidBody.forEachPart(func);
	this->forEachPartInChildren(func);
}

// expects a function of type void(Part&)
template<typename Func>
void Physical::forEachPartInThisAndChildren(const Func& func) {
	this->rigidBody.forEachPart(func);
	this->forEachPartInChildren(func);
}

// expects a function of type void(const Physical& parent, const ConnectedPhysical& child)
template<typename Func>
void Physical::forEachHardConstraintInChildren(const Func& func) const {
	for(const ConnectedPhysical& conPhys : this->childPhysicals) {
		func(*this, conPhys);
		conPhys.forEachHardConstraintInChildren(func);
	}
}

// expects a function of type void(Physical& parent, ConnectedPhysical& child)
template<typename Func>
void Physical::forEachHardConstraintInChildren(const Func& func) {
	for(ConnectedPhysical& conPhys : this->childPhysicals) {
		func(*this, conPhys);
		conPhys.forEachHardConstraintInChildren(func);
	}
}

// used to precompute all the internal motions in the MotorizedPhysical
class InternalMotionTree {
	const MotorizedPhysical* motorPhys;
	MonotonicTree<RelativeMotion> relativeMotionTree;
public:

	inline InternalMotionTree(const MotorizedPhysical* motorPhys, MonotonicTree<RelativeMotion>&& relativeMotionTree) : motorPhys(motorPhys), relativeMotionTree(std::move(relativeMotionTree)) {}

	// connectedPhysFunc is of the form void(const T& currentValue, const ConnectedPhysical& currentPhysical)
	template<typename F>
	void recurse(F connectedPhysFunc) const {
		motorPhys->mutuallyRecurse(relativeMotionTree, connectedPhysFunc);
	}

	// returns the total mass, the center of mass relative to the MotorizedPhysical's CFrame, and the motion of the center of mass in that CFrame
	std::tuple<double, Vec3, TranslationalMotion> getInternalMotionOfCenterOfMass() const;
	// normalizes this motion tree relative to this->getInternalMotionOfCenterOfMass()
	NormalizedInternalMotionTree normalizeCenterOfMass() &&;
};

// Same as InternalMotionTree, but relative to the position and speed of the center of mass
class NormalizedInternalMotionTree {
	friend class InternalMotionTree;

	const MotorizedPhysical* motorPhys;
	MonotonicTree<RelativeMotion> relativeMotionTree;
public:
	double totalMass;
	Vec3 centerOfMass;
	TranslationalMotion motionOfCenterOfMass;
private:
	inline NormalizedInternalMotionTree(const MotorizedPhysical* motorPhys, MonotonicTree<RelativeMotion>&& relativeMotionTree, double totalMass, Vec3 centerOfMass, TranslationalMotion motionOfCenterOfMass) : 
		motorPhys(motorPhys), 
		relativeMotionTree(std::move(relativeMotionTree)),
		totalMass(totalMass),
		centerOfMass(centerOfMass),
		motionOfCenterOfMass(motionOfCenterOfMass) {}

public:

	// connectedPhysFunc is of the form void(const T& currentValue, const ConnectedPhysical& currentPhysical)
	template<typename F>
	void recurse(F connectedPhysFunc) const {
		motorPhys->mutuallyRecurse(relativeMotionTree, connectedPhysFunc);
	}

	SymmetricMat3 getInertia() const;
	FullTaylor<SymmetricMat3> getInertiaDerivatives() const;
};

